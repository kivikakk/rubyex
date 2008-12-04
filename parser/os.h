#ifndef OS_H
#define OS_H

#include <iterator>

#ifdef __MACH__
template <class A, class B> bool operator ==(const std::reverse_iterator<A> &x, const std::reverse_iterator<B> &y) {
	return x.base() == y.base();
}

template <class A, class B> bool operator !=(const std::reverse_iterator<A> &x, const std::reverse_iterator<B> &y) {
	return x.base() != y.base();
}
#endif

#endif

