/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
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
#ifndef LIBBITCOIN_DATABASE_BLOCK_RESULT_HPP_
#define LIBBITCOIN_DATABASE_BLOCK_RESULT_HPP_

#ifdef BITPRIM_DB_LEGACY

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/database/define.hpp>
#include <bitcoin/database/memory/memory.hpp>

namespace libbitcoin {
namespace database {

/// Deferred read block result.
class BCD_API block_result
{
public:
    block_result();
    block_result(const memory_ptr slab);
    block_result(const memory_ptr slab, hash_digest&& hash, uint32_t height);
    block_result(const memory_ptr slab, const hash_digest& hash,
        uint32_t height);

    /// True if this block result is valid (found).
    operator bool() const;

    /// Reset the slab pointer so that no lock is held.
    void reset();

    /// The block header hash (from cache).
    const hash_digest& hash() const;

    /// The block header.
    chain::header header() const;

    /// The height of this block in the chain.
    size_t height() const;

    /// The header.bits of this block.
    uint32_t bits() const;

    /// The header.timestamp of this block.
    uint32_t timestamp() const;

    /// The header.version of this block.
    uint32_t version() const;

    /// The number of transactions in this block.
    size_t transaction_count() const;

    /// A transaction hash where index < transaction_count.
    hash_digest transaction_hash(size_t index) const;

    /// An ordered set of all transaction hashes in the block.
    hash_list transaction_hashes() const;

    uint64_t serialized_size() const;

private:
    memory_ptr slab_;
    const uint32_t height_;
    const hash_digest hash_;
};

} // namespace database
} // namespace libbitcoin

#endif // BITPRIM_DB_LEGACY

#endif // LIBBITCOIN_DATABASE_BLOCK_RESULT_HPP_
