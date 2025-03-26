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
 */

#pragma once
#include <dataframe/Serie.h>
#include <map>

namespace df {

/**
 * @brief A utility class for encoding categorical string values as numeric IDs
 *
 * This class is used to convert between string labels and numeric indices,
 * which is necessary for algorithms like Random Forest that require numeric
 * input.
 */
class LabelEncoder {
  public:
    /**
     * @brief Default constructor
     */
    LabelEncoder() = default;

    /**
     * @brief Fit the encoder to a Serie of strings
     * @param serie Serie<std::string> containing the values to encode
     * @return Reference to this encoder for method chaining
     */
    LabelEncoder &fit(const df::Serie<std::string> &serie);

    /**
     * @brief Transform a Serie of strings to their corresponding numeric IDs
     * @param serie Serie<std::string> to transform
     * @return Serie<double> with the encoded values
     * @throw std::runtime_error if the encoder hasn't been fitted or encounters
     * unknown values
     */
    df::Serie<double> transform(const df::Serie<std::string> &serie) const;

    /**
     * @brief Fit to a Serie of strings and transform in one step
     * @param serie Serie<std::string> to fit and transform
     * @return Serie<double> with the encoded values
     */
    df::Serie<double> fit_transform(const df::Serie<std::string> &serie);

    /**
     * @brief Convert numeric IDs back to their original string values
     * @param serie Serie<double> containing the IDs to convert
     * @return Serie<std::string> with the original string values
     * @throw std::runtime_error if the encoder hasn't been fitted or encounters
     * unknown IDs
     */
    df::Serie<std::string>
    inverse_transform(const df::Serie<double> &serie) const;

    /**
     * @brief Check if the encoder has been fitted
     * @return True if the encoder has been fitted, false otherwise
     */
    bool is_fitted() const;

    /**
     * @brief Get the number of unique categories
     * @return The number of unique categories
     */
    size_t num_categories() const;

    /**
     * @brief Get all unique categories as a vector
     * @return Vector of unique category strings
     */
    std::vector<std::string> categories() const;

  private:
    std::map<std::string, size_t> string_to_id_;
    std::map<size_t, std::string> id_to_string_;
    bool fitted_ = false;
};

} // namespace df

#include "inline/label_encoder.hxx"