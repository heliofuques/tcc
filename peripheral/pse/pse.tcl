imodelnewperipheral  \
    -name        nonSecToSec \
    -library     peripheral \
    -version     0.1 \
    -constructor constructor\
    -visibility    visible 

# Output bus size is 32 bits
set outputBusWidth  32




#GENERAL
# Define enable net input port
imodeladdnetport -name enable -type input -updatefunction updateEnable

# Define formals for specifying the output master and slave bus port location and size
imodeladdformal -name portSize    -type integer
imodeladdformal -name enableBridge -type bool



## Secure registers
imodeladdbusslaveport -name readSecurePort \
                      -mustbeconnected

imodeladdaddressblock -name regs   \
                       -port readSecurePort \
                       -offset 0x0  \
                       -width 16    \
                       -size 8

imodeladdmmregister  -name ns_interrupt_ack \
                     -addressblock readSecurePort/regs \
                     -readfunction   interruptRead \
                     -writefunction  interruptWrite             

imodeladdlocalmemory -name ns_data_header \
                     -addressblock readSecurePort/regs \
                     -readfunction dataHeaderRead\
                     -offset 1 \
                     -size 2

imodeladdlocalmemory -name ns_data \
                     -addressblock readSecurePort/regs \
                     -readfunction dataRead\
                     -offset 3 \
                     -size 1

### 
imodeladdnetport -name newMessageAvailable -type output 


# Non secure registers - RNS1

imodeladdbusslaveport -name rxNonSecurePort1 

imodeladdaddressblock -name rxRegs   \
                       -port rxNonSecurePort1 \
                       -offset 0x0  \
                       -width 8    \
                       -size 12

imodeladdmmregister  -name rx_req \
                     -addressblock rxNonSecurePort1/rxRegs \
                     -writefunction writeTxReq \
                     -offset 0x0 \
                     -access w 

imodeladdmmregister  -name rx_ack \
                     -addressblock rxNonSecurePort1/rxRegs \
                     -readfunction  readTxAck \
                     -offset 0x1 \
                     -access r


imodeladdmmregister  -name dataTxLocalHeader \
                     -addressblock rxNonSecurePort1/rxRegs \
                     -writefunction  txWriteHeader \
                     -offset 0x2 \
                     -width 2 \
                     -access w

imodeladdmmregister  -name dataTxLocal \
                     -addressblock rxNonSecurePort1/rxRegs \
                     -writefunction  txWrite \
                     -offset 0x4 \
                     -access w

                     
# Non secure registers - RNS2

imodeladdbusslaveport -name rxNonSecurePort2

imodeladdaddressblock -name rxRegs   \
                       -port rxNonSecurePort2 \
                       -offset 0x0  \
                       -width 8    \
                       -size 12

imodeladdmmregister  -name rx_req \
                     -addressblock rxNonSecurePort2/rxRegs \
                     -writefunction writeTxReqRNS2 \
                     -offset 0x0 \
                     -access w 

imodeladdmmregister  -name rx_ack\
                     -addressblock rxNonSecurePort2/rxRegs \
                     -readfunction  readTxAckRNS2 \
                     -offset 0x1 \
                     -access r


imodeladdmmregister  -name dataTxLocalHeader \
                     -addressblock rxNonSecurePort2/rxRegs \
                     -writefunction  txWriteHeader \
                     -offset 0x2 \
                     -width 2 \
                     -access w

imodeladdmmregister  -name dataTxLocal \
                     -addressblock rxNonSecurePort2/rxRegs \
                     -writefunction  txWrite \
                     -offset 0x4 \
                     -access w
