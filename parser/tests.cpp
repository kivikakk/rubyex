#include "main.h"
#include "global.h"
#include "tests.h"

Test::Test(): end_marker(true) { }
Test::Test(const std::string &_name, const test_pointer &_test): end_marker(false), name(_name), test(_test) { }

int assertions, assertion_successes;
bool test_succeeding;

int tests_all()
{
  Test *ptr = tests;
  int tests_run = 0, tests_succeed = 0;
  assertions = 0; assertion_successes = 0;

  while (!ptr->end_marker) {
    Test test = *ptr;
    test_succeeding = true;

    std::cout << "test: \"" << test.name << "\"" << std::endl;
    try {
      (*test.test)();
    } catch (std::exception &e) {
      std::cerr << "exception failure: " << e.what() << std::endl;
      test_succeeding = false;
    } catch (...) {
      test_succeeding = false;
    }

    if (test_succeeding)
      tests_succeed++;
    else
      std::cerr << "test: \"" << test.name << "\": failure" << std::endl;

    ptr++, tests_run++;
  }

  yylex_destroy();

  std::cerr << tests_run << " test(s) run, " << tests_succeed << " test(s) succeeded." << std::endl;
  std::cerr << assertions << " assertion(s), " << assertion_successes << " assertion(s) succeeded." << std::endl;
  if (tests_run != tests_succeed)
    return 1;
  return 0;
}

bool assert_throw(int line, const char *exp, bool eval)
{
  ++assertions;
  std::cout << "asserting: " << exp << std::endl;
  if (!eval) {
    std::cerr << "assertion failure: line " << line << ": " << exp << std::endl;
    test_succeeding = false;
    return false;
  }
  ++assertion_successes;
  return true;
}

void _literals()
{
  BEGIN(integer, "42", 1);
  $(LiteralTypedExpr<int>, integer_expr, integer[0]);
  ASSERT(integer_expr->value == 42);

  BEGIN(negative_integer, "-19", 1);
  $(LiteralTypedExpr<int>, negative_integer_expr, negative_integer[0]);
  ASSERT(negative_integer_expr->value == -19);

  BEGIN(floating, "93.12", 1);
  $(LiteralTypedExpr<double>, floating_expr, floating[0]);
  ASSERT(floating_expr->value == 93.12);	// this is quite brittle. what if it's 93.11999...?

  BEGIN(negative_floating, "-100.1", 1);
  $(LiteralTypedExpr<double>, negative_floating_expr, negative_floating[0]);
  ASSERT(negative_floating_expr->value == -100.1);	// also brittle

  BEGIN(dbl_string, "\"hi!\"", 1);
  $(LiteralTypedExpr<std::string>, dbl_string_expr, dbl_string[0]);
  ASSERT(dbl_string_expr->value == "hi!");

  BEGIN(dbl_string_nl, "\"hi!\nhow are you?\"", 1);
  $(LiteralTypedExpr<std::string>, dbl_string_nl_expr, dbl_string_nl[0]);
  ASSERT(dbl_string_nl_expr->value == "hi!\nhow are you?");

  BEGIN(bool_true, "true", 1);
  $(LiteralTypedExpr<bool>, bool_true_expr, bool_true[0]);
  ASSERT(bool_true_expr->value == true);

  BEGIN(bool_false, "false", 1);
  $(LiteralTypedExpr<bool>, bool_false_expr, bool_false[0]);
  ASSERT(bool_false_expr->value == false);
}

void _identifier()
{
  BEGIN(p, "abc", 1);
  $(IdentifierExpr, id, p[0]);
  ASSERT(id->id == "abc");
}

void _symbol()
{
  BEGIN(p_regular, ":bob", 1);
  $(SymbolExpr, sym_regular, p_regular[0]);
  ASSERT(sym_regular->symbol == "bob");

  BEGIN(p_question, ":bob?", 1);
  $(SymbolExpr, sym_question, p_question[0]);
  ASSERT(sym_question->symbol == "bob?");

  BEGIN(p_exclaim, ":bob!", 1);
  $(SymbolExpr, sym_exclaim, p_exclaim[0]);
  ASSERT(sym_exclaim->symbol == "bob!");

  BEGIN(p_dbq, ":\"double quoted string\"", 1);
  $(SymbolExpr, sym_dbq, p_dbq[0]);
  ASSERT(sym_dbq->symbol == "double quoted string");

  BEGIN(p_nl, ":\"with\nnewline\"", 1);
  $(SymbolExpr, sym_nl, p_nl[0]);
  ASSERT(sym_nl->symbol == "with\nnewline");

  ASSERT_NOPARSE(":1one");
  ASSERT_NOPARSE(":!jelly");
}

void _call()
{
  BEGIN(p_explicit, "joe()", 1);
  $(FuncCallExpr, func_explicit, p_explicit[0]);
  ASSERT(func_explicit->name == "joe");
  ASSERT(func_explicit->args.size() == 0);
  ASSERT(func_explicit->target == NULL);
  ASSERT(func_explicit->block == NULL);

  BEGIN(p_questioning, "krista? why", 1);
  $(FuncCallExpr, func_questioning, p_questioning[0]);
  ASSERT(func_questioning->name == "krista?");
  ASSERT(func_questioning->args.size() == 1);
  $(IdentifierExpr, func_questioning_why, *func_questioning->args.begin());
  ASSERT(func_questioning_why->id == "why");
  ASSERT(func_questioning->target == NULL);
  ASSERT(func_questioning->block == NULL);

  BEGIN(p_exclaiming, "nicolas! hey", 1);
  $(FuncCallExpr, func_exclaiming, p_exclaiming[0]);
  ASSERT(func_exclaiming->name == "nicolas!");
  ASSERT(func_exclaiming->args.size() == 1);
  $(IdentifierExpr, func_exclaiming_why, *func_exclaiming->args.begin());
  ASSERT(func_exclaiming_why->id == "hey");
  ASSERT(func_exclaiming->target == NULL);
  ASSERT(func_exclaiming->block == NULL);

  BEGIN(p_mixing, "a!b?c!", 1);
  $(FuncCallExpr, m_a, p_mixing[0]);
  ASSERT(m_a->target == NULL);
  ASSERT(m_a->block == NULL);
  ASSERT(m_a->name == "a!");
  ASSERT(m_a->args.size() == 1);
  $(FuncCallExpr, m_b, *m_a->args.begin());
  ASSERT(m_b->target == NULL);
  ASSERT(m_b->block == NULL);
  ASSERT(m_b->name == "b?");
  ASSERT(m_b->args.size() == 1);
  $(FuncCallExpr, m_c, *m_b->args.begin());
  ASSERT(m_c->target == NULL);
  ASSERT(m_c->block == NULL);
  ASSERT(m_c->name == "c!");
  ASSERT(m_c->args.size() == 0);

  BEGIN(p_one_implicit, "alex hybrid", 1);
  $(FuncCallExpr, func_one_implicit, p_one_implicit[0]);
  ASSERT(func_one_implicit->name == "alex");
  ASSERT(func_one_implicit->args.size() == 1);
  ASSERT(func_one_implicit->target == NULL);
  ASSERT(func_one_implicit->block == NULL);
  $(IdentifierExpr, func_one_implicit_arg, *func_one_implicit->args.begin());
  ASSERT(func_one_implicit_arg->id == "hybrid");

  BEGIN(p_one_explicit, "morgan(ipsicle)", 1);
  $(FuncCallExpr, func_one_explicit, p_one_explicit[0]);
  ASSERT(func_one_explicit->name == "morgan");
  ASSERT(func_one_explicit->args.size() == 1);
  ASSERT(func_one_explicit->target == NULL);
  ASSERT(func_one_explicit->block == NULL);
  $(IdentifierExpr, func_one_explicit_arg, *func_one_explicit->args.begin());
  ASSERT(func_one_explicit_arg->id == "ipsicle");

  BEGIN(p_many_nested, "lorraine hamster(wilfred, (enigma), murray(darling, basin, river), peacock), 92", 1);
  $(FuncCallExpr, func_many_nested, p_many_nested[0]);
  ASSERT(func_many_nested->name == "lorraine");
  ASSERT(func_many_nested->args.size() == 2);
  ASSERT(func_many_nested->target == NULL);
  ASSERT(func_many_nested->block == NULL);
  std::list<Expr *>::iterator itp = func_many_nested->args.begin();
  {
    // First argument.
    $(FuncCallExpr, fce, *itp);
    ASSERT(fce->name == "hamster");
    ASSERT(fce->args.size() == 4);

    std::list<Expr *>::iterator sub_itp = fce->args.begin();
    { $(IdentifierExpr, idx, *sub_itp); ASSERT(idx->id == "wilfred"); }
    ++sub_itp;
    { $(IdentifierExpr, idx, *sub_itp); ASSERT(idx->id == "enigma"); }
    ++sub_itp;
    {
      $(FuncCallExpr, fcx, *sub_itp);
      ASSERT(fcx->name == "murray");
      ASSERT(fcx->args.size() == 3);

      std::list<Expr *>::iterator murray_itp = fcx->args.begin();
      { $(IdentifierExpr, idx, *murray_itp); ASSERT(idx->id == "darling"); }
      ++murray_itp;
      { $(IdentifierExpr, idx, *murray_itp); ASSERT(idx->id == "basin"); }
      ++murray_itp;
      { $(IdentifierExpr, idx, *murray_itp); ASSERT(idx->id == "river"); }
    }
    ++sub_itp;
    { $(IdentifierExpr, idx, *sub_itp); ASSERT(idx->id == "peacock"); }
  }
  ++itp;
  { $(LiteralTypedExpr<int>, integer, *itp); ASSERT(integer->value == 92); }

  ASSERT_NOPARSE("baked!?");
  ASSERT_NOPARSE("fried(joker");
}

void _assignment()
{
  BEGIN(p, "a = b", 1);
  $(AssignmentExpr, assign, p[0]);
  ASSERT(assign->name == "a");
  $(IdentifierExpr, rval, assign->value);
  ASSERT(rval->id == "b");

  ASSERT_NOPARSE("4 = b");
  ASSERT_NOPARSE("nox! = pox");
  ASSERT_NOPARSE("tipsy? = turvy");
}

void _method_call()
{
  BEGIN(p_regular, "abc.def", 1);
  $(FuncCallExpr, regular, p_regular[0]);
  $(IdentifierExpr, lval, regular->target);
  ASSERT(lval->id == "abc");
  ASSERT(regular->name == "def");
  ASSERT(regular->args.size() == 0);
  ASSERT(regular->block == NULL);

  BEGIN(p_left_exp, "(norman.expose).friedlich", 1);
  $(FuncCallExpr, left_exp, p_left_exp[0]);
  $(FuncCallExpr, left_call, left_exp->target);
  $(IdentifierExpr, left_lval, left_call->target);
  ASSERT(left_lval->id == "norman");
  ASSERT(left_call->name == "expose");
  ASSERT(left_call->args.size() == 0);
  ASSERT(left_call->block == NULL);
  ASSERT(left_exp->name == "friedlich");
  ASSERT(left_exp->args.size() == 0);
  ASSERT(left_exp->block == NULL);

  BEGIN(p_the_lot, "(joe.wilfred tsonga()).sucked at the_open, today", 1);
  $(FuncCallExpr, tennis, p_the_lot[0]);
  $(FuncCallExpr, player, tennis->target);
  $(IdentifierExpr, first, player->target);
  ASSERT(first->id == "joe");
  ASSERT(player->name == "wilfred");
  ASSERT(player->block == NULL);
  ASSERT(player->args.size() == 1);
  $(FuncCallExpr, lastname, *player->args.begin());
  ASSERT(lastname->target == NULL);
  ASSERT(lastname->name == "tsonga");
  ASSERT(lastname->args.size() == 0);
  ASSERT(lastname->block == NULL);
  ASSERT(tennis->name == "sucked");
  ASSERT(tennis->args.size() == 1);
  $(FuncCallExpr, whence, *tennis->args.begin());
  ASSERT(whence->target == NULL);
  ASSERT(whence->name == "at");
  ASSERT(whence->block == NULL);
  ASSERT(whence->args.size() == 2);

  std::list<Expr *>::iterator it = whence->args.begin();
  $(IdentifierExpr, location, *it);
  ASSERT(location->id == "the_open");
  ++it;
  $(IdentifierExpr, time, *it);
  ASSERT(time->id == "today");

  ASSERT_NOPARSE("abc.!def");
  ASSERT_NOPARSE("abc.(ghi)");
  ASSERT_NOPARSE("abc.4");
}

void _operations()
{
  // Operations are in fact method calls, so we'll test them afterward.
  BEGIN(p_simple, "1 + 2", 1);
  $(FuncCallExpr, simple, p_simple[0]);
  $(LiteralTypedExpr<int>, simple_1, simple->target);
  ASSERT(simple_1->value == 1);
  ASSERT(simple->name == "+");
  ASSERT(simple->args.size() == 1);
  $(LiteralTypedExpr<int>, simple_2, *simple->args.begin());
  ASSERT(simple_2->value == 2);

  BEGIN(p_complex, "(9 - 3 / (5 * -negate_it)) + 0", 1);
  // (9.-(3./(5.*(2.-@)))).+(0)
  // parentheses inserted above as leaving 1 + 2 + 3 has ambiguous result:
  // 1.+(2.+(3)) or (1.+(2)).+(3) or .. etc.
  $(FuncCallExpr, expr_plus_0, p_complex[0]);
  $(FuncCallExpr, nine_minus_expr, expr_plus_0->target);
  $(LiteralTypedExpr<int>, nine, nine_minus_expr->target);
  ASSERT(nine->value == 9);
  ASSERT(nine_minus_expr->name == "-");
  ASSERT(nine_minus_expr->args.size() == 1);
  $(FuncCallExpr, three_divides_expr, *nine_minus_expr->args.begin());
  $(LiteralTypedExpr<int>, three, three_divides_expr->target);
  ASSERT(three->value == 3);
  ASSERT(three_divides_expr->name == "/");
  ASSERT(three_divides_expr->args.size() == 1);
  $(FuncCallExpr, five_times_expr, *three_divides_expr->args.begin());
  $(LiteralTypedExpr<int>, five, five_times_expr->target);
  ASSERT(five->value == 5);
  ASSERT(five_times_expr->name == "*");
  ASSERT(five_times_expr->args.size() == 1);
  $(FuncCallExpr, negative_negate_expr, *five_times_expr->args.begin());
  $(IdentifierExpr, negate_it, negative_negate_expr->target);
  ASSERT(negate_it->id == "negate_it");
  ASSERT(negative_negate_expr->name == "-@");
  ASSERT(negative_negate_expr->args.size() == 0);
  ASSERT(expr_plus_0->name == "+");
  ASSERT(expr_plus_0->args.size() == 1);
  $(LiteralTypedExpr<int>, zero, *expr_plus_0->args.begin());
  ASSERT(zero->value == 0);
}

void _operator_precedence()
{
  // a - 5 		-> a.-(5)
  // a -5		-> a(-5)
  // a-5		-> a.-(5)

  BEGIN(p1, "a - 5", 1);
  $(FuncCallExpr, p1e, p1[0]);
  $(IdentifierExpr, p1_a, p1e->target);
  ASSERT(p1_a->id == "a");
  ASSERT(p1e->name == "-");
  ASSERT(p1e->args.size() == 1);
  $(LiteralTypedExpr<int>, p1_5, *p1e->args.begin());
  ASSERT(p1_5->value == 5);

  BEGIN(p2, "a -5", 1);
  $(FuncCallExpr, p2e, p2[0]);
  ASSERT(p2e->name == "a");
  ASSERT(p2e->args.size() == 1);
  $(LiteralTypedExpr<int>, p2_n5, *p2e->args.begin());
  ASSERT(p2_n5->value == -5);

  BEGIN(p3, "a-5", 1);
  $(FuncCallExpr, p3e, p3[0]);
  $(IdentifierExpr, p3_a, p3e->target);
  ASSERT(p3_a->id == "a");
  ASSERT(p3e->name == "-");
  ASSERT(p3e->args.size() == 1);
  $(LiteralTypedExpr<int>, p3_5, *p3e->args.begin());
  ASSERT(p3_5->value == 5);
}

void _block()
{
  BEGIN(p_empty, "call {}", 1);
  $(FuncCallExpr, empty, p_empty[0]);
  ASSERT(empty->target == NULL);
  ASSERT(empty->name == "call");
  ASSERT(empty->args.size() == 0);
  ASSERT(empty->block);
  ASSERT(empty->block->proc->expressions.size() == 0);

  BEGIN(p_one, "call {boo}", 1);
  $(FuncCallExpr, one, p_one[0]);
  ASSERT(one->target == NULL);
  ASSERT(one->name == "call");
  ASSERT(one->args.size() == 0);
  ASSERT(one->block);
  ASSERT(one->block->proc->expressions.size() == 1);
  $(IdentifierExpr, one_boo, *one->block->proc->expressions.begin());
  ASSERT(one_boo->id == "boo");

  BEGIN(p_one_do, "call do boo end", 1);
  $(FuncCallExpr, one_do, p_one[0]);
  ASSERT(one_do->target == NULL);
  ASSERT(one_do->name == "call");
  ASSERT(one_do->args.size() == 0);
  ASSERT(one_do->block);
  ASSERT(one_do->block->proc->expressions.size() == 1);
  $(IdentifierExpr, one_do_boo, *one->block->proc->expressions.begin());
  ASSERT(one_do_boo->id == "boo");

  BEGIN(p_one_nl, "call {\n\tboo\n}\n", 1);
  $(FuncCallExpr, one_nl, p_one[0]);
  ASSERT(one_nl->target == NULL);
  ASSERT(one_nl->name == "call");
  ASSERT(one_nl->args.size() == 0);
  ASSERT(one_nl->block);
  ASSERT(one_nl->block->proc->expressions.size() == 1);
  $(IdentifierExpr, one_nl_boo, *one->block->proc->expressions.begin());
  ASSERT(one_nl_boo->id == "boo");

  BEGIN(p_nested, "call do nesting { well_this {}; that {}; the_other {\nthat\nchocolate};; }; done end", 1);
  // what a joke to test. Just watch this:
  $(FuncCallExpr, nested, p_nested[0]);
  ASSERT(nested->target == NULL);
  ASSERT(nested->name == "call");
  ASSERT(nested->args.size() == 0);
  ASSERT(nested->block);
  ASSERT(nested->block->proc->expressions.size() == 2);
  
  std::list<Expr *>::iterator it = nested->block->proc->expressions.begin();
  {
    $(FuncCallExpr, nesting, *it);
    ASSERT(nesting->target == NULL);
    ASSERT(nesting->name == "nesting");
    ASSERT(nesting->args.size() == 0);
    ASSERT(nesting->block);
    ASSERT(nesting->block->proc->expressions.size() == 3);
    std::list<Expr *>::iterator sit = nesting->block->proc->expressions.begin();
    {
      $(FuncCallExpr, well_this, *sit);
      ASSERT(well_this->name == "well_this");
      ASSERT(well_this->args.size() == 0);
      ASSERT(well_this->block);
      ASSERT(well_this->block->proc->expressions.size() == 0);
    }
    ++sit;
    {
      $(FuncCallExpr, that, *sit);
      ASSERT(that->name == "that");
      ASSERT(that->args.size() == 0);
      ASSERT(that->block);
      ASSERT(that->block->proc->expressions.size() == 0);
    }
    ++sit;
    {
      $(FuncCallExpr, the_other, *sit);
      ASSERT(the_other->name == "the_other");
      ASSERT(the_other->args.size() == 0);
      ASSERT(the_other->block);
      ASSERT(the_other->block->proc->expressions.size() == 2);

      std::list<Expr *>::iterator tsit = the_other->block->proc->expressions.begin();
      {
	$(IdentifierExpr, that, *tsit);
	ASSERT(that->id == "that");
      }
      ++tsit;
      {
	$(IdentifierExpr, then, *tsit);
	ASSERT(then->id == "chocolate");
      }
    }
  }
  ++it;
  {
    $(IdentifierExpr, done, *it);
    ASSERT(done->id == "done");
  }
}

void _block_args()
{
  BEGIN(p, "simple {||}", 1);
  $(FuncCallExpr, simple, p[0]);
  ASSERT(simple->target == NULL);
  ASSERT(simple->name == "simple");
  $(BlockExpr, simple_block, simple->block);
  ASSERT(simple_block->args.size() == 0);
  ASSERT(simple_block->proc->expressions.size() == 0);


  BEGIN(p_one, "one {|a| pink}", 1);
  $(FuncCallExpr, one, p_one[0]);
  ASSERT(one->target == NULL);
  ASSERT(one->name == "one");
  $(BlockExpr, one_block, one->block);
  ASSERT(one_block->args.size() == 1);
  $(IdentifierExpr, a, *one_block->args.begin());
  ASSERT(a->id == "a");
  ASSERT(one_block->proc->expressions.size() == 1);
  $(IdentifierExpr, pink, *one_block->proc->expressions.begin());
  ASSERT(pink->id == "pink");

  BEGIN(p_many, "many do |x, y, z|\n\tx * y - z\nend", 1);
  $(FuncCallExpr, many, p_many[0]);
  ASSERT(many->target == NULL);
  ASSERT(many->name == "many");
  $(BlockExpr, many_block, many->block);
  ASSERT(many_block->args.size() == 3);
  std::list<IdentifierExpr *>::iterator it = many_block->args.begin();
  { $(IdentifierExpr, itx, *it); ASSERT(itx->id == "x"); } ++it;
  { $(IdentifierExpr, itx, *it); ASSERT(itx->id == "y"); } ++it;
  { $(IdentifierExpr, itx, *it); ASSERT(itx->id == "z"); }
  ASSERT(many_block->proc->expressions.size() == 1);
  $(FuncCallExpr, xyz, *many_block->proc->expressions.begin());
  $(FuncCallExpr, x_y, xyz->target);
  $(IdentifierExpr, _x, x_y->target);
  ASSERT(_x->id == "x");
  ASSERT(x_y->name == "*");
  ASSERT(x_y->args.size() == 1);
  $(IdentifierExpr, _y, *x_y->args.begin());
  ASSERT(_y->id == "y");
  ASSERT(xyz->name == "-");
  ASSERT(xyz->args.size() == 1);
  $(IdentifierExpr, _z, *xyz->args.begin());
  ASSERT(_z->id == "z");
}

void _multi_call()
{
  BEGIN(p_simple, "a.b.c", 1);
  $(FuncCallExpr, ab_c, p_simple[0]);
  $(FuncCallExpr, a_b, ab_c->target);
  $(IdentifierExpr, a, a_b->target);
  ASSERT(a->id == "a");
  ASSERT(a_b->name == "b");
  ASSERT(ab_c->name == "c");

  BEGIN(p_args, "a(b).c", 1);
  $(FuncCallExpr, args, p_args[0]);
  $(FuncCallExpr, args_a_of_b, args->target);
  ASSERT(args_a_of_b->name == "a");
  ASSERT(args_a_of_b->args.size() == 1);
  $(IdentifierExpr, args_b, *args_a_of_b->args.begin());
  ASSERT(args_b->id == "b");
  ASSERT(args->name == "c");

  BEGIN(p_spacer, "a b.c(d{f(g)}).e(h){q{m()}}", 1);
  $(FuncCallExpr, spacer, p_spacer[0]);
  ASSERT(spacer->name == "a");
  ASSERT(spacer->args.size() == 1);
  $(FuncCallExpr, bc_e, *spacer->args.begin());
  $(FuncCallExpr, b_c, bc_e->target);
  $(IdentifierExpr, b, b_c->target);
  ASSERT(b->id == "b");
  ASSERT(b_c->name == "c");
  ASSERT(b_c->args.size() == 1);
  $(FuncCallExpr, d_fg, *b_c->args.begin());
  ASSERT(d_fg->name == "d");
  $(BlockExpr, _fg, d_fg->block);
  ASSERT(_fg->proc->expressions.size() == 1);
  $(FuncCallExpr, f_g, *_fg->proc->expressions.begin());
  ASSERT(f_g->name == "f");
  ASSERT(f_g->args.size() == 1);
  $(IdentifierExpr, g, *f_g->args.begin());
  ASSERT(g->id == "g");
  ASSERT(bc_e->name == "e");
  ASSERT(bc_e->args.size() == 1);
  $(IdentifierExpr, h, *bc_e->args.begin());
  ASSERT(h->id == "h");
  $(BlockExpr, _qm, bc_e->block);
  ASSERT(_qm->proc->expressions.size() == 1);
  $(FuncCallExpr, qm, *_qm->proc->expressions.begin());
  ASSERT(qm->name == "q");
  $(BlockExpr, _m, qm->block);
  ASSERT(_m->proc->expressions.size() == 1);
  $(FuncCallExpr, m, *_m->proc->expressions.begin());
  ASSERT(m->name == "m");
  ASSERT(m->args.size() == 0);
}

Test tests[] = {
  TEST(literals),
  TEST(identifier),
  TEST(symbol),
  TEST(call),
  TEST(assignment),
  TEST(method_call),
  TEST(operations),
  TEST(operator_precedence),
  TEST(block),
  TEST(block_args),
  TEST(multi_call),
  TEST_END()
};

