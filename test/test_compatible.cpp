/***********************************************
The MIT License (MIT)

Copyright (c) 2012 Athrun Arthur <athrunarthur@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*************************************************/
#define BOOST_TEST_MODULE test_ff
#include <boost/test/included/unit_test.hpp>
#include "ff.h"

template<class T1, class T2, class F>
void check_and(F &&f, bool req)
{
  bool b = ff::internal::is_compatible_then<F, T1, T2>::is_cpt_with_and;
  BOOST_CHECK(b == req);
}
template<class T1, class T2, class F>
void check_or(F &&f, bool req)
{
  bool b = ff::internal::is_compatible_then<F, T1, T2>::is_cpt_with_or;
  BOOST_CHECK(b == req);
}


BOOST_AUTO_TEST_SUITE(minimal_test)

BOOST_AUTO_TEST_CASE(simple_test_case)
{
    check_and<int, int>([](int a, int b){}, true);
    check_or<int, int>([](int, std::tuple<int, int>){}, true);
}

BOOST_AUTO_TEST_CASE(void_test_case)
{
  check_and<void, int>([](int){}, true);
  check_or<void, int>([](bool, int){}, true);
}

BOOST_AUTO_TEST_CASE(void2_test_case)
{
  check_and<void, void>([](){}, true);
  check_or<void, void>([](){}, true);
}
BOOST_AUTO_TEST_SUITE_END()
