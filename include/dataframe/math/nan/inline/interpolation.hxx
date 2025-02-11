/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

namespace df {
namespace nan {

template <typename T> class NanInterpolator {
  public:
    static Serie<T> fill_nan(const Serie<T> &serie,
                             FillMethod method = FillMethod::LINEAR) {
        if (serie.empty()) {
            return serie;
        }

        // Create a copy of the input serie
        std::vector<T> result(serie.data());

        // Find spans of NaN values
        std::vector<std::pair<size_t, size_t>> nan_spans;
        find_nan_spans(result, nan_spans);

        // Process each span based on chosen method
        for (const auto &span : nan_spans) {
            fill_span(result, span.first, span.second, method);
        }

        return Serie<T>(result);
    }

  private:
    static void find_nan_spans(const std::vector<T> &data,
                               std::vector<std::pair<size_t, size_t>> &spans) {
        size_t start = 0;
        bool in_span = false;

        for (size_t i = 0; i < data.size(); ++i) {
            if (std::isnan(data[i])) {
                if (!in_span) {
                    start = i;
                    in_span = true;
                }
            } else if (in_span) {
                spans.emplace_back(start, i - 1);
                in_span = false;
            }
        }

        // Handle span at end of series
        if (in_span) {
            spans.emplace_back(start, data.size() - 1);
        }
    }

    static void fill_span(std::vector<T> &data, size_t start, size_t end,
                          FillMethod method) {
        // Find valid values before and after the span
        std::optional<T> before =
            start > 0 ? std::optional<T>(data[start - 1]) : std::nullopt;
        std::optional<T> after = end < data.size() - 1
                                     ? std::optional<T>(data[end + 1])
                                     : std::nullopt;

        switch (method) {
        case FillMethod::LINEAR:
            fill_linear(data, start, end, before, after);
            break;
        case FillMethod::NEAREST:
            fill_nearest(data, start, end, before, after);
            break;
        case FillMethod::PREVIOUS:
            fill_previous(data, start, end, before);
            break;
        case FillMethod::NEXT:
            fill_next(data, start, end, after);
            break;
        case FillMethod::MEAN:
            fill_mean(data, start, end, before, after);
            break;
        }
    }

    static void fill_linear(std::vector<T> &data, size_t start, size_t end,
                            std::optional<T> before, std::optional<T> after) {
        // Handle edge cases
        if (!before && !after) {
            std::fill(data.begin() + start, data.begin() + end + 1, T{0});
            return;
        }
        if (!before)
            before = after;
        if (!after)
            after = before;

        // Linear interpolation
        T start_val = *before;
        T end_val = *after;
        size_t steps = end - start + 2;
        T step_size = (end_val - start_val) / static_cast<T>(steps);

        for (size_t i = 0; i <= (end - start); ++i) {
            data[start + i] = start_val + step_size * static_cast<T>(i + 1);
        }
    }

    static void fill_nearest(std::vector<T> &data, size_t start, size_t end,
                             std::optional<T> before, std::optional<T> after) {
        if (!before && !after) {
            std::fill(data.begin() + start, data.begin() + end + 1, T{0});
            return;
        }

        // Use nearest valid value
        if (!before) {
            std::fill(data.begin() + start, data.begin() + end + 1, *after);
        } else if (!after) {
            std::fill(data.begin() + start, data.begin() + end + 1, *before);
        } else {
            size_t mid = (start + end) / 2;
            std::fill(data.begin() + start, data.begin() + mid + 1, *before);
            std::fill(data.begin() + mid + 1, data.begin() + end + 1, *after);
        }
    }

    static void fill_previous(std::vector<T> &data, size_t start, size_t end,
                              std::optional<T> before) {
        T value = before.value_or(T{0});
        std::fill(data.begin() + start, data.begin() + end + 1, value);
    }

    static void fill_next(std::vector<T> &data, size_t start, size_t end,
                          std::optional<T> after) {
        T value = after.value_or(T{0});
        std::fill(data.begin() + start, data.begin() + end + 1, value);
    }

    static void fill_mean(std::vector<T> &data, size_t start, size_t end,
                          std::optional<T> before, std::optional<T> after) {
        if (!before && !after) {
            std::fill(data.begin() + start, data.begin() + end + 1, T{0});
            return;
        }

        T mean;
        if (!before) {
            mean = *after;
        } else if (!after) {
            mean = *before;
        } else {
            mean = (*before + *after) / static_cast<T>(2);
        }

        std::fill(data.begin() + start, data.begin() + end + 1, mean);
    }
};

// Helper function
template <typename T>
inline Serie<T> interpolate(const Serie<T> &serie, FillMethod method) {
    return NanInterpolator<T>::fill_nan(serie, method);
}

} // namespace nan
} // namespace df