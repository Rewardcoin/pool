#include <Python.h>

static const int64_t COIN = 100000000;
static const int64_t minimumSubsidy = 1.0 * COIN;
static const int64_t nGenesisBlockRewardCoin = 1 * COIN;
static const int64_t nPremine = 21000000 * COIN;

int64 static GetBlockSubsidy(int nHeight){
    
    
    if (nHeight == 0)
    {
        return nGenesisBlockRewardCoin;
    }
    
    int64 nSubsidy = minimumSubsidy;
    
    if(nHeight <= 10)
    {
        nSubsidy = nPremine;
    }
    else if(nHeight<=43210) 
    {
        nSubsidy=200*COIN;
    }
    else if(nHeight<=86410)  
    {
        nSubsidy=2000*COIN;
    }
    else if(nHeight<=129610)
    {
        nSubsidy=50*COIN;
    }
    else if(nHeight<=172810)
    {
        nSubsidy=800*COIN;
    }
    else if(nHeight<=216010)
    {
        nSubsidy=20*COIN;
    }
    else if(nHeight<=259210)
    {
        nSubsidy=1500*COIN;
    }
    else if(nHeight<=518410)
    {
        nSubsidy=20*COIN;
    }
    else if(nHeight<=777610)
    {
        nSubsidy=10*COIN;
    }
    else if(nHeight<=1036810)
    {
        nSubsidy=5*COIN;
    }
    else if(nHeight<=1296010)
    {
        nSubsidy=3*COIN;
    }
    else  
    {
        nSubsidy=1*COIN;
    }

    return nSubsidy;
}


int64_t static GetBlockBaseValue(int nBits, int nHeight)
{

    return GetBlockSubsidy(nHeight);
}

static PyObject *sophiacoin_subsidy_getblockbasevalue(PyObject *self, PyObject *args)
{
    int input_bits;
    int input_height;
    if (!PyArg_ParseTuple(args, "ii", &input_bits, &input_height))
        return NULL;
    long long output = GetBlockBaseValue(input_bits, input_height);
    return Py_BuildValue("L", output);
}

static PyMethodDef sophiacoin_subsidy_methods[] = {
    { "getBlockBaseValue", sophiacoin_subsidy_getblockbasevalue, METH_VARARGS, "Returns the block value" },
    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initsophiacoin_subsidy(void) {
    (void) Py_InitModule("sophiacoin_subsidy", sophiacoin_subsidy_methods);
}
