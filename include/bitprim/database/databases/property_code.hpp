/**
 * Copyright (c) 2016-2018 Bitprim Inc.
 *
 * This file is part of Bitprim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef BITPRIM_DATABASE_PROPERTY_CODE_HPP_
#define BITPRIM_DATABASE_PROPERTY_CODE_HPP_

namespace libbitcoin {
namespace database {

enum class property_code {
    db_mode = 0, //enum
    last_indexed_height = 1, //uint32_t
    indexing_completed = 3 // bool
};

enum class db_mode_code {
    db_new = 0,
    db_new_with_blocks = 1,
    db_new_full = 2,
    db_new_full_async = 3
};

} // namespace database
} // namespace libbitcoin

#endif // BITPRIM_DATABASE_PROPERTY_CODE_HPP_
