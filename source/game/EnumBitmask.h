/*---------------------------------------------------------------------------------------------
*  Copyright (c) Nicolas Jinchereau. All rights reserved.
*  Licensed under the MIT License. See License.txt in the project root for license information.
*--------------------------------------------------------------------------------------------*/

#pragma once
#include <type_traits>

#define ENUM_BITMASK(e) \
inline e operator|(e left, e right) { return (e)((std::underlying_type<e>::type)left | (std::underlying_type<e>::type)right); } \
inline e operator&(e left, e right) { return (e)((std::underlying_type<e>::type)left & (std::underlying_type<e>::type)right); } \
inline std::underlying_type<e>::type operator&(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left & right; } \
inline e operator^(e left, e right) { return (e)((std::underlying_type<e>::type)left ^ (std::underlying_type<e>::type)right); } \
inline e operator~(e value) { return (e)(~(std::underlying_type<e>::type)value); } \
inline e& operator|=(e& left, e right) { return (left = (left | right)); } \
inline e& operator&=(e& left, e right) { return (left = (left & right)); } \
inline e& operator^=(e& left, e right) { return (left = (left ^ right)); } \
inline e operator+(e left, std::underlying_type<e>::type right) { return (e)((std::underlying_type<e>::type)left + right); } \
inline e operator-(e left, std::underlying_type<e>::type right) { return (e)((std::underlying_type<e>::type)left - right); } \
inline e operator+(e left, e right) { return (e)((std::underlying_type<e>::type)left + (std::underlying_type<e>::type)right); } \
inline e operator-(e left, e right) { return (e)((std::underlying_type<e>::type)left - (std::underlying_type<e>::type)right); } \
inline e& operator+=(e& left, std::underlying_type<e>::type right) { return (left = (e)((std::underlying_type<e>::type)left + right)); } \
inline e& operator-=(e& left, std::underlying_type<e>::type right) { return (left = (e)((std::underlying_type<e>::type)left - right)); } \
inline e& operator+=(e& left, e right) { return (left = (e)((std::underlying_type<e>::type)left + (std::underlying_type<e>::type)right)); } \
inline e& operator-=(e& left, e right) { return (left = (e)((std::underlying_type<e>::type)left - (std::underlying_type<e>::type)right)); } \
inline bool operator==(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left == right; } \
inline bool operator!=(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left != right; } \
inline bool operator< (e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left <  right; } \
inline bool operator<=(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left <= right; } \
inline bool operator> (e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left >  right; } \
inline bool operator>=(e left, std::underlying_type<e>::type right) { return (std::underlying_type<e>::type)left >= right; } \
inline bool operator==(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left == right; } \
inline bool operator!=(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left != right; } \
inline bool operator< (std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left <  right; } \
inline bool operator<=(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left <= right; } \
inline bool operator> (std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left >  right; } \
inline bool operator>=(std::underlying_type<e>::type left, e right) { return (std::underlying_type<e>::type)left >= right; }
