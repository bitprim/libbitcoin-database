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
#ifndef BITPRIM_DATABASE_HISTORY_DATABASE_IPP_
#define BITPRIM_DATABASE_HISTORY_DATABASE_IPP_

// #include <boost/filesystem.hpp>
// #include <boost/range/adaptor/reversed.hpp>
// #include <boost/interprocess/mapped_region.hpp>

// #include <lmdb.h>

// #include <bitcoin/bitcoin.hpp>
// #include <bitcoin/database/define.hpp>

// #include <bitprim/database/databases/result_code.hpp>
// #include <bitprim/database/databases/tools.hpp>
// #include <bitprim/database/databases/utxo_entry.hpp>
// #include <bitprim/database/databases/history_entry.hpp>


namespace libbitcoin {
namespace database {


#if defined(BITPRIM_DB_NEW_FULL)

template <typename Clock>
result_code internal_database_basis<Clock>::insert_input_history(hash_digest const& tx_hash,uint32_t height, uint32_t index, chain::input const& input, MDB_txn* db_txn) {
    
    //TODO store inpoint
    auto const inpoint = chain::input_point {tx_hash, index};
    auto const& prevout = input.previous_output();

    if (prevout.validation.cache.is_valid()) {
        // This results in a complete and unambiguous history for the
        // address since standard outputs contain unambiguous address data.
        for (auto const& address : prevout.validation.cache.addresses()) {
            
            auto key_arr = address.hash();                                    
            MDB_val key {key_arr.size(), key_arr.data()};   

            auto valuearr = history_entry::factory_to_data(chain::point_kind::spend, height, index, prevout.checksum());
            MDB_val value {valuearr.size(), valuearr.data()};

            auto res = mdb_put(db_txn, dbi_history_db_, &key, &value, 0);
            if (res == MDB_KEYEXIST) {
                LOG_INFO(LOG_DATABASE) << "Duplicate key inserting history [insert_input_history] " << res;        
                return result_code::duplicated_key;
            }
            if (res != MDB_SUCCESS) {
                LOG_INFO(LOG_DATABASE) << "Error inserting history [insert_input_history] " << res;        
                return result_code::other;
            }
        
        }
    } else {
        // For any p2pk spend this creates no record (insufficient data).
        // For any p2kh spend this creates the ambiguous p2sh address,
        // which significantly expands the size of the history store.
        // These are tradeoffs when no prevout is cached (checkpoint sync).
        bool valid = true;
        for (auto const& address : input.addresses()) {
            if ( ! address) {
                valid = false;
                break;
            }
        }
        
        if (valid) {
            for (auto const& address : input.addresses()) {
                
                auto key_arr = address.hash();                                    
                MDB_val key {key_arr.size(), key_arr.data()};   

                auto valuearr = history_entry::factory_to_data(chain::point_kind::spend, height, index, prevout.checksum());
                MDB_val value {valuearr.size(), valuearr.data()};

                auto res = mdb_put(db_txn, dbi_history_db_, &key, &value, 0);
                if (res == MDB_KEYEXIST) {
                    LOG_INFO(LOG_DATABASE) << "Duplicate key inserting history [insert_input_history] " << res;        
                    return result_code::duplicated_key;
                }
                if (res != MDB_SUCCESS) {
                    LOG_INFO(LOG_DATABASE) << "Error inserting history [insert_input_history] " << res;        
                    return result_code::other;
                }

            }
        } else {
            //During an IBD with checkpoints some previous output info is missing.
            //We can recover it by accessing the database
            
            auto const& tx = get_transaction(prevout.hash(), db_txn);

            if (tx.is_valid()) {

                auto const& out_output = tx.outputs()[prevout.index()];

                for (auto const& address : out_output.addresses()) {

                    auto key_arr = address.hash();                                    
                    MDB_val key {key_arr.size(), key_arr.data()};   

                    auto valuearr = history_entry::factory_to_data(chain::point_kind::spend, height, index, prevout.checksum());
                    MDB_val value {valuearr.size(), valuearr.data()};

                    auto res = mdb_put(db_txn, dbi_history_db_, &key, &value, 0);
                    if (res == MDB_KEYEXIST) {
                        LOG_INFO(LOG_DATABASE) << "Duplicate key inserting history [insert_input_history] " << res;        
                        return result_code::duplicated_key;
                    }
                    if (res != MDB_SUCCESS) {
                        LOG_INFO(LOG_DATABASE) << "Error inserting history [insert_input_history] " << res;        
                        return result_code::other;
                    }
                }


            }
        }
    }

    return result_code::success;
}

template <typename Clock>
result_code internal_database_basis<Clock>::insert_output_history(hash_digest const& tx_hash,uint32_t height, uint32_t index, chain::output const& output, MDB_txn* db_txn ) {
    
    //TODO Store outpoint
    auto const outpoint = chain::output_point {tx_hash, index};
    auto const value = output.value();

    // Standard outputs contain unambiguous address data.
    for (auto const& address : output.addresses()) {
        
        auto key_arr = address.hash();                                    
        MDB_val key {key_arr.size(), key_arr.data()};   

        auto valuearr = history_entry::factory_to_data(chain::point_kind::output, height, index, value);
        MDB_val value {valuearr.size(), valuearr.data()};

        auto res = mdb_put(db_txn, dbi_history_db_, &key, &value, 0);
        if (res == MDB_KEYEXIST) {
            LOG_INFO(LOG_DATABASE) << "Duplicate key inserting history [insert_output_history] " << res;        
            return result_code::duplicated_key;
        }
        if (res != MDB_SUCCESS) {
            LOG_INFO(LOG_DATABASE) << "Error inserting history [insert_output_history] " << res;        
            return result_code::other;
        }
    }

    return result_code::success;
}

#endif //BITPRIM_NEW_DB_FULL


} // namespace database
} // namespace libbitcoin

#endif // BITPRIM_DATABASE_HISTORY_DATABASE_IPP_