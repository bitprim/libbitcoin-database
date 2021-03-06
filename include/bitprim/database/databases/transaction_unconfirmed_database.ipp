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
#ifndef BITPRIM_DATABASE_TRANSACTION_UNCONFIRMED_DATABASE_HPP_
#define BITPRIM_DATABASE_TRANSACTION_UNCONFIRMED_DATABASE_HPP_

namespace libbitcoin {
namespace database {


#if defined(BITPRIM_DB_NEW_FULL)


template <typename Clock>
transaction_unconfirmed_entry internal_database_basis<Clock>::get_transaction_unconfirmed(hash_digest const& hash) const {
    
    MDB_txn* db_txn;
    auto res = mdb_txn_begin(env_, NULL, MDB_RDONLY, &db_txn);
    if (res != MDB_SUCCESS) {
        return {};
    }

    auto const& ret = get_transaction_unconfirmed(hash, db_txn);

    if (mdb_txn_commit(db_txn) != MDB_SUCCESS) {
        return {};
    }

    return ret;
}

template <typename Clock>
transaction_unconfirmed_entry internal_database_basis<Clock>::get_transaction_unconfirmed(hash_digest const& hash, MDB_txn* db_txn) const {
    MDB_val key {hash.size(), const_cast<hash_digest&>(hash).data()};
    MDB_val value;

    if (mdb_get(db_txn, dbi_transaction_unconfirmed_db_, &key, &value) != MDB_SUCCESS) {
        return {};
    }

    auto data = db_value_to_data_chunk(value);
    auto res = transaction_unconfirmed_entry::factory_from_data(data);

    return res;
}


template <typename Clock>
std::vector<transaction_unconfirmed_entry> internal_database_basis<Clock>::get_all_transaction_unconfirmed() const {

    std::vector<transaction_unconfirmed_entry> result;

    MDB_txn* db_txn;
    auto res = mdb_txn_begin(env_, NULL, MDB_RDONLY, &db_txn);
    if (res != MDB_SUCCESS) {
        return result;
    }

    MDB_cursor* cursor;

    if (mdb_cursor_open(db_txn, dbi_transaction_unconfirmed_db_, &cursor) != MDB_SUCCESS) {
        mdb_txn_commit(db_txn);
        return result;
    }

    MDB_val key;
    MDB_val value;
    
    
    int rc;
    if ((rc = mdb_cursor_get(cursor, &key, &value, MDB_NEXT)) == 0) {
       
        auto data = db_value_to_data_chunk(value);
        auto res = transaction_unconfirmed_entry::factory_from_data(data);
        result.push_back(res);

        while ((rc = mdb_cursor_get(cursor, &key, &value, MDB_NEXT)) == 0) {
            auto data = db_value_to_data_chunk(value);
            auto res = transaction_unconfirmed_entry::factory_from_data(data);
            result.push_back(res);
        }
    } 
    
    mdb_cursor_close(cursor);

    if (mdb_txn_commit(db_txn) != MDB_SUCCESS) {
        return result;
    }

    return result;
}



template <typename Clock>
result_code internal_database_basis<Clock>::remove_transaction_unconfirmed(hash_digest const& tx_id,  MDB_txn* db_txn) {

    MDB_val key {tx_id.size(), const_cast<hash_digest&>(tx_id).data()};

    auto res = mdb_del(db_txn, dbi_transaction_unconfirmed_db_, &key, NULL);
    if (res == MDB_NOTFOUND) {
        //LOG_DEBUG(LOG_DATABASE) << "Key not found deleting transaction unconfirmed DB in LMDB [remove_transaction_unconfirmed] - mdb_del: " << res;
        return result_code::key_not_found;
    }
    if (res != MDB_SUCCESS) {
        LOG_INFO(LOG_DATABASE) << "Error deleting transaction unconfirmed DB in LMDB [remove_transaction_unconfirmed] - mdb_del: " << res;
        return result_code::other;
    }

    return result_code::success;
}

template <typename Clock>
uint32_t internal_database_basis<Clock>::get_clock_now() {
    auto const now = std::chrono::high_resolution_clock::now();
    return static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
}


template <typename Clock>
result_code internal_database_basis<Clock>::insert_transaction_unconfirmed(chain::transaction const& tx, uint32_t height, MDB_txn* db_txn) {
    
    uint32_t arrival_time = get_clock_now();
    
    auto key_arr = tx.hash();                                    //TODO(fernando): podría estar afuera de la DBTx
    MDB_val key {key_arr.size(), key_arr.data()};

    auto value_arr = transaction_unconfirmed_entry::factory_to_data(tx, arrival_time, height);
    MDB_val value {value_arr.size(), value_arr.data()}; 

    auto res = mdb_put(db_txn, dbi_transaction_unconfirmed_db_, &key, &value, MDB_NOOVERWRITE);
    if (res == MDB_KEYEXIST) {
        LOG_INFO(LOG_DATABASE) << "Duplicate key in Transaction Unconfirmed DB [insert_transaction_unconfirmed] " << res;
        return result_code::duplicated_key;
    }        

    if (res != MDB_SUCCESS) {
        LOG_INFO(LOG_DATABASE) << "Error saving in Transaction Unconfirmed DB [insert_transaction_unconfirmed] " << res;
        return result_code::other;
    }

    return result_code::success;
}


#endif //BITPRIM_NEW_DB_FULL


} // namespace database
} // namespace libbitcoin

#endif // BITPRIM_DATABASE_TRANSACTION_UNCONFIRMED_DATABASE_HPP_
