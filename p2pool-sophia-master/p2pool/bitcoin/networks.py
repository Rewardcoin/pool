import os
import platform

from twisted.internet import defer

from . import data
from p2pool.util import math, pack, jsonrpc

@defer.inlineCallbacks
def check_genesis_block(bitcoind, genesis_block_hash):
    try:
        yield bitcoind.rpc_getblock(genesis_block_hash)
    except jsonrpc.Error_for_code(-5):
        defer.returnValue(False)
    else:
        defer.returnValue(True)

nets = dict(

    sophiacoin=math.Object(
        P2P_PREFIX='c3f1a405'.decode('hex'),
        P2P_PORT=9696,
        ADDRESS_VERSION=63,
        RPC_PORT=9695,
        RPC_CHECK=defer.inlineCallbacks(lambda bitcoind: defer.returnValue(
            'SophiaCoinaddress' in (yield bitcoind.rpc_help()) and
            not (yield bitcoind.rpc_getinfo())['testnet']
        )),
        SUBSIDY_FUNC=lambda nBits, height: __import__('sophiacoin_subsidy').getBlockBaseValue(nBits, height+1),
        POW_FUNC=lambda data: pack.IntType(256).unpack(__import__('sophiacoin_hash').getHash(data, 80)),
        BLOCK_PERIOD=60, # s
        SYMBOL='SPC',
        CONF_FILE_FUNC=lambda: os.path.join(os.path.join(os.environ['APPDATA'], 'SophiaCoin') if platform.system() == 'Windows' else os.path.expanduser('~/Library/Application Support/SophiaCoin/') if platform.system() == 'Darwin' else os.path.expanduser('~/.SophiaCoin'), 'SophiaCoin.conf'),
        BLOCK_EXPLORER_URL_PREFIX='',
        ADDRESS_EXPLORER_URL_PREFIX='',
        TX_EXPLORER_URL_PREFIX=,
        SANE_TARGET_RANGE=(2**256//2**32//1000 - 1, 2**256//2**20 - 1),
        DUMB_SCRYPT_DIFF=256,
        DUST_THRESHOLD=0.001e8,
    ),

    sophiacoin_testnet=math.Object(
        P2P_PREFIX='fba206d3'.decode('hex'),
        P2P_PORT=19695,
        ADDRESS_VERSION=111,
        RPC_PORT=19696,
        RPC_CHECK=defer.inlineCallbacks(lambda bitcoind: defer.returnValue(
            'SophiaCoinaddress' in (yield bitcoind.rpc_help()) and
            (yield bitcoind.rpc_getinfo())['testnet']
        )),
        SUBSIDY_FUNC=lambda nBits, height: __import__('sophiacoin_subsidy').getBlockBaseValue(nBits, height+1),
        POW_FUNC=lambda data: pack.IntType(256).unpack(__import__('sophiacoin_hash').getHash(data, 80)),
        BLOCK_PERIOD=60, # s
        SYMBOL='SPC',
        CONF_FILE_FUNC=lambda: os.path.join(os.path.join(os.environ['APPDATA'], 'SophiaCoin') if platform.system() == 'Windows' else os.path.expanduser('~/Library/Application Support/SophiaCoin/') if platform.system() == 'Darwin' else os.path.expanduser('~/.SophiaCoin'), 'SophiaCoin.conf'),
        BLOCK_EXPLORER_URL_PREFIX='',
        ADDRESS_EXPLORER_URL_PREFIX='',
        TX_EXPLORER_URL_PREFIX='',
        SANE_TARGET_RANGE=(2**256//2**32//1000 - 1, 2**256//2**20 - 1),
        DUMB_SCRYPT_DIFF=256,
        DUST_THRESHOLD=0.001e8,
    ),

)
for net_name, net in nets.iteritems():
    net.NAME = net_name
