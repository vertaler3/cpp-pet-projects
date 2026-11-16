#pragma once

#include <cstddef>
#include <deque>

template<class T, class Comp>
std::deque<T> Merge(const std::deque<T>& half1, const std::deque<T>& half2, const Comp& comparator) {
    std::deque<T> ans;
    size_t i = 0;
    size_t j = 0;

    while (i != half1.size() && j != half2.size()) {
        if (comparator(half2[j], half1[i])) {
            ans.push_back(half2[j]);
            j++;
        } else {
            ans.push_back(half1[i]);
            i++;
        }
    }
    if (j == half2.size()) {
        for (; i < half1.size(); i++) {
            ans.push_back(half1[i]);
        }
    }

    if (i == half1.size()) {
        for (; j < half2.size(); j++) {
            ans.push_back(half2[j]);
        }
    }
    return ans;
}

template<class T, class Comp>
std::deque<T> MergeSort(const std::deque<T>& src, const Comp& comparator) {
    if (src.size() <= 1) {
        return src;
    }
    const size_t mid = src.size() / 2;
    const std::deque<T> left_sorted = MergeSort(std::deque<T>(src.begin(), src.begin() + mid), comparator);
    const std::deque<T> right_sorted = MergeSort(std::deque<T>(src.begin() + mid, src.end()), comparator);

    return Merge(left_sorted, right_sorted, comparator);
}
