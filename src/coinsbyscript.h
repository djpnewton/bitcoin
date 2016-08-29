// Copyright (c) 2014-2016 The Bitcoin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_COINSBYSCRIPT_H
#define BITCOIN_COINSBYSCRIPT_H

#include "coins.h"
#include "dbwrapper.h"
#include "primitives/transaction.h"
#include "serialize.h"
#include "uint256.h"

class CCoinsViewDB;
class CCoinsViewByScriptDB;
class CScript;

class CCoinsByScript
{
public:
    // unspent transaction outputs
    std::set<COutPoint> setCoins;

    // empty constructor
    CCoinsByScript() { }

    bool IsEmpty() const {
        return (setCoins.empty());
    }

    void swap(CCoinsByScript &to) {
        to.setCoins.swap(setCoins);
    }

    ADD_SERIALIZE_METHODS;
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(setCoins);
    }
};

typedef std::map<uint160, CCoinsByScript> CCoinsMapByScript; // uint160 = hash of script

/** Adds a memory cache for coins by address */
class CCoinsViewByScript
{
private:
    CCoinsViewByScriptDB *base;

public:
    CCoinsMapByScript cacheCoinsByScript; // accessed also from CCoinsViewDB in txdb.cpp DN:TODO not for long
    CCoinsViewByScript(CCoinsViewByScriptDB* baseIn);

    bool GetCoinsByScript(const CScript &script, CCoinsByScript &coins);

    // Return a modifiable reference to a CCoinsByScript.
    CCoinsByScript &GetCoinsByScript(const CScript &script, bool fRequireExisting = true);

    static uint160 getKey(const CScript &script); // we use the hash of the script as key in the database

private:
    CCoinsMapByScript::iterator FetchCoinsByScript(const CScript &script, bool fRequireExisting);
};

//DN: TODO
#ifdef blah
/** Cursor for iterating over CoinsViewByScript state */
class CCoinsViewByScriptDBCursor
{
public:
    CCoinsViewCursor(const uint256 &hashBlockIn): hashBlock(hashBlockIn) {}
    virtual ~CCoinsViewByScriptDBCursor();

    virtual bool GetKey(uint160 &key) const = 0;
    virtual bool GetValue(CCoinsByScript &coins) const = 0;
    /* Don't care about GetKeySize here */
    virtual unsigned int GetValueSize() const = 0;

    virtual bool Valid() const = 0;
    virtual void Next() = 0;

    //! Get best block at the time this cursor was created
    const uint256 &GetBestBlock() const { return hashBlock; } //DN: needed?
private:
    uint256 hashBlock;
};
#endif

//DN: TODO
/** CCoinsViewByScript backed by the coinsbyscript database (coinsbyscript/) */
class CCoinsViewByScriptDB 
{
private:
    CCoinsViewByScript* pcoinsViewByScript;
protected:
    CDBWrapper db;
public:
    CCoinsViewByScriptDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

    bool GetCoinsByHashOfScript(const uint160 &hash, CCoinsByScript &coins) const;
    bool BatchWrite(CCoinsMap &mapCoins, const uint256 &hashBlock);
    bool WriteFlag(const std::string &name, bool fValue);
    bool ReadFlag(const std::string &name, bool &fValue);
    bool DeleteAllCoinsByScript();   // removes txoutsbyaddressindex
    bool GenerateAllCoinsByScript(CCoinsViewDB* coinsIn); // creates txoutsbyaddressindex
    void SetCoinsViewByScript(CCoinsViewByScript* pcoinsViewByScriptIn);
    CDBIterator *RawCursor() const;
    //DN:TODO
    //CCoinsViewByScriptDBCursor *Cursor() const;
};

#endif // BITCOIN_COINSBYSCRIPT_H
