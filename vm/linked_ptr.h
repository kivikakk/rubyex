#ifndef LINKED_PTR_H
#define LINKED_PTR_H

template <class X>
class linked_ptr
{
  public:
#ifndef NO_MEMBER_TEMPLATES
#  define TEMPLATE_FUNCTION template <class Y>
    TEMPLATE_FUNCTION friend class linked_ptr<Y>;
#else
#  define TEMPLATE FUNCTION
    typedef X Y;
#endif

    typedef X element_type;

    explicit linked_ptr(X* p = 0) throw() : ptr(p)
    {
      prev = next = this;
    }

    ~linked_ptr()
    {
      release();
    }

    linked_ptr(const linked_ptr &r) throw()
    {
      acquire(r);
    }

    linked_ptr &operator=(const linked_ptr &r)
    {
      if (this != &r) {
	release();
	acquire(r);
      }

      return *this;
    }

#ifndef NO_MEMBER_TEMPLATES
    template <class Y> friend class linked_ptr<Y>;
    template <class Y> linked_ptr(const linked_ptr<Y> &r) throw()
    {
      acquire(r);
    }
    template <class Y> linked_ptr &operator=(const linked_ptr<Y> &r)
    {
      if (this != &r) {
	release();
	acquire(r);
      }

      return *this;
    }
#endif

    X &operator*() const throw()	{ return *ptr; }
    X *operator->() const throw()	{ return ptr; }
    X *get() const throw()		{ return ptr; }
    bool unique() const throw()		{ return prev ? prev == this : true; }

  private:
    X *ptr;
    mutable const linked_ptr *prev;
    mutable const linked_ptr *next;

    void acquire(const linked_ptr &r) throw()
    {
      ptr = r.ptr;
      next = r.next;
      next->prev = this;
      prev = &r;
      r.next = this;
    }

#ifndef NO_MEMBER_TEMPLATES
    template <class Y> void acquire(const linked_ptr<Y> &r) throw()
    {
      ptr = r.ptr;
      next = r.next;
      next->prev = this;
      prev = &r;
      r.next = this;
    }
#endif

    void release()
    {
      if (unique())
	delete ptr;
      else {
	prev->next = next;
	next->prev = prev;
	next = prev = 0;
      }
      ptr = 0;
    }
};

#endif

