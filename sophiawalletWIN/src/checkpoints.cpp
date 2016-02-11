// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "main.h"
#include "uint256.h"

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    // How many times we expect transactions after the last checkpoint to
    // be slower. This number is a compromise, as it can't be accurate for
    // every system. When reindexing from a fast disk with a slow CPU, it
    // can be up to 20, while when downloading from a slow network with a
    // fast multicore CPU, it won't be much higher than 1.
    static const double fSigcheckVerificationFactor = 5.0;

    struct CCheckpointData {
        const MapCheckpoints *mapCheckpoints;
        int64 nTimeLastCheckpoint;
        int64 nTransactionsLastCheckpoint;
        double fTransactionsPerDay;
    };

    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (     0, uint256("0x00000a54055b541049bbac30a37bdc44ccd803e36ea86081a41191731cfa7cea"))
        (     10, uint256("0x000005ea899a1033982a5f7a51df2c95ce6e831bb198e350e1d967134c5b2d8c"))
        (     50, uint256("0x000000203882cb902f947a9eb578dd71f00d1e5cbe10a7abcd33ef3484b79143"))
        (     101, uint256("0x00000139f2c4d90935eb9237febcdd7874f14a998f421f71bd44eff1145efb43"))
        (     161, uint256("0x000000550e9c22f62e38ea55690c03027b444ae511a33c5be83b76d28b02e965"))
        (     267, uint256("0x000000ef2a7ccbf196d4153a69e5cf3e2470720693b83e2ef7bbc9e42ab894da"))

        ;

    static const CCheckpointData data = {
        &mapCheckpoints,
      1453667244, // * UNIX timestamp of last checkpoint block ("sophiacoind getblock 00000fb14b24d6e978bef9ed02fdcf0585ac5ac31e42a7e3a858f94a6dba505d")
      268,     // * total number of transactions between genesis and last checkpoint 
                    //   (the tx=... number in the SetBestChain debug.log lines)
      6000     // * estimated number of transactions per day after checkpoint
    };

    static MapCheckpoints mapCheckpointsTestnet = 
        boost::assign::map_list_of
        (     0, uint256("0x00000a54055b541049bbac30a37bdc44ccd803e36ea86081a41191731cfa7cea"))
        (     10, uint256("0x000005ea899a1033982a5f7a51df2c95ce6e831bb198e350e1d967134c5b2d8c"))
        (     50, uint256("0x000000203882cb902f947a9eb578dd71f00d1e5cbe10a7abcd33ef3484b79143"))
        (     101, uint256("0x00000139f2c4d90935eb9237febcdd7874f14a998f421f71bd44eff1145efb43"))
        (     161, uint256("0x000000550e9c22f62e38ea55690c03027b444ae511a33c5be83b76d28b02e965"))
        (     267, uint256("0x000000ef2a7ccbf196d4153a69e5cf3e2470720693b83e2ef7bbc9e42ab894da"))
       
        ;
    static const CCheckpointData dataTestnet = {
        &mapCheckpointsTestnet,
      // 0, // * UNIX timestamp of last checkpoint block ("sophiacoind getblock 00000fb14b24d6e978bef9ed02fdcf0585ac5ac31e42a7e3a858f94a6dba505d")
      // 141,     // * total number of transactions between genesis and last checkpoint 
                    //   (the tx=... number in the SetBestChain debug.log lines)
      // 6000     // * estimated number of transactions per day after checkpoint
    };

    const CCheckpointData &Checkpoints() {
        if (fTestNet)
            return dataTestnet;
        else
            return data;
    }

    bool CheckBlock(int nHeight, const uint256& hash)
    {
        if (!GetBoolArg("-checkpoints", true))
            return true;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    // Guess how far we are in the verification process at the given block index
    double GuessVerificationProgress(CBlockIndex *pindex) {
        if (pindex==NULL)
            return 0.0;

        int64 nNow = time(NULL);

        double fWorkBefore = 0.0; // Amount of work done before pindex
        double fWorkAfter = 0.0;  // Amount of work left after pindex (estimated)
        // Work is defined as: 1.0 per transaction before the last checkoint, and
        // fSigcheckVerificationFactor per transaction after.

        const CCheckpointData &data = Checkpoints();

        if (pindex->nChainTx <= data.nTransactionsLastCheckpoint) {
            double nCheapBefore = pindex->nChainTx;
            double nCheapAfter = data.nTransactionsLastCheckpoint - pindex->nChainTx;
            double nExpensiveAfter = (nNow - data.nTimeLastCheckpoint)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore;
            fWorkAfter = nCheapAfter + nExpensiveAfter*fSigcheckVerificationFactor;
        } else {
            double nCheapBefore = data.nTransactionsLastCheckpoint;
            double nExpensiveBefore = pindex->nChainTx - data.nTransactionsLastCheckpoint;
            double nExpensiveAfter = (nNow - pindex->nTime)/86400.0*data.fTransactionsPerDay;
            fWorkBefore = nCheapBefore + nExpensiveBefore*fSigcheckVerificationFactor;
            fWorkAfter = nExpensiveAfter*fSigcheckVerificationFactor;
        }

        return fWorkBefore / (fWorkBefore + fWorkAfter);
    }

    int GetTotalBlocksEstimate()
    {
        if (!GetBoolArg("-checkpoints", true))
            return 0;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        if (!GetBoolArg("-checkpoints", true))
            return NULL;

        const MapCheckpoints& checkpoints = *Checkpoints().mapCheckpoints;

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }
}
