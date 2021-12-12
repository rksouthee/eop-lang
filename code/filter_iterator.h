#ifndef EOP_LANG_FILTER_ITERATOR_H
#define EOP_LANG_FILTER_ITERATOR_H

#include <iterator>

template <typename I, typename P>
class Filter_iterator
{
public:
	using value_type = typename std::iterator_traits<I>::value_type;
	using reference = typename std::iterator_traits<I>::reference;
	using pointer = typename std::iterator_traits<I>::pointer;
	using difference_type = typename std::iterator_traits<I>::difference_type;
	using iterator_category = std::forward_iterator_tag;

private:
	I m_iter;
	I m_end;
	P m_pred;

public:
	Filter_iterator() = default;
	Filter_iterator(const I& iter, const I& end, const P& pred);

	auto base() const -> I;

	auto operator*() const -> reference;
	auto operator->() const -> pointer;

	auto operator++() -> Filter_iterator&;
	auto operator++(int) -> Filter_iterator;
};

template <typename I, typename P>
auto operator==(const Filter_iterator<I, P>& x, const Filter_iterator<I, P>& y) -> bool;

template <typename I, typename P>
auto operator!=(const Filter_iterator<I, P>& x, const Filter_iterator<I, P>& y) -> bool;

template <typename I, typename P>
Filter_iterator<I, P>::Filter_iterator(const I& iter, const I& end, const P& pred) :
	m_iter(iter),
	m_end(end),
	m_pred(pred)
{
	m_iter = std::find_if(m_iter, m_end, m_pred);
}

template <typename I, typename P>
auto Filter_iterator<I, P>::base() const -> I
{
	return m_iter;
}

template <typename I, typename P>
auto Filter_iterator<I, P>::operator*() const -> reference
{
	return *m_iter;
}

template <typename I, typename P>
auto Filter_iterator<I, P>::operator->() const -> pointer
{
	return &**this;
}

template <typename I, typename P>
auto Filter_iterator<I, P>::operator++() -> Filter_iterator&
{
	m_iter = std::find_if(std::next(m_iter), m_end, m_pred);
	return *this;
}

template <typename I, typename P>
auto Filter_iterator<I, P>::operator++(int) -> Filter_iterator
{
	Filter_iterator tmp = *this;
	++*this;
	return tmp;
}

template <typename I, typename P>
auto operator==(const Filter_iterator<I, P>& x, const Filter_iterator<I, P>& y) -> bool
{
	return x.base() == y.base();
}

template <typename I, typename P>
auto operator!=(const Filter_iterator<I, P>& x, const Filter_iterator<I, P>& y) -> bool
{
	return !(x == y);
}

#endif
