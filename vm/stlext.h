#ifndef __STLEXT_H__
#define __STLEXT_H__

#include <functional>

template<typename pair_type>
struct second_equal_to : public std::unary_function<const pair_type &, bool>
{
  second_equal_to(const typename pair_type::second_type &__store): store(__store)
  { }

  bool operator()(const pair_type& __v) const
  {
    return __v.second == store;
  }

  protected:
    const typename pair_type::second_type &store;
};

/*template <typename pair_type>
struct select2nd : public std::unary_function<const pair_type &, typename const pair_type::second_type &>
{
  const typename pair_type::second_type &operator()(const pair_type &v) const
  {
    return v.second;
  }
}; */

#endif

