/*
 * Copyright (c) 2005-2017 Imperas Software Ltd., www.imperas.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


////////////////////////////////////////////////////////////////////////////////
//
//                W R I T T E N   B Y   I M P E R A S   I G E N
//
//                             Version 20170201.0
//
////////////////////////////////////////////////////////////////////////////////

#ifndef PSE_MACROS_IGEN_H
#define PSE_MACROS_IGEN_H
// Before including this file in the application, define the indicated macros
// to fix the base address of each slave port.
// Set the macro 'READSECUREPORT' to the base of port 'readSecurePort'
#ifndef READSECUREPORT
    #error READSECUREPORT is undefined.It needs to be set to the port base address
#endif
#define READSECUREPORT_REGS_NS_INTERRUPT_ACK    (READSECUREPORT + 0x0)

#define READSECUREPORT_REGS_NS_DATA_HEADER          (READSECUREPORT + 0x1)
#define READSECUREPORT_REGS_NS_DATA_HEADER_BYTES    0x2

#define READSECUREPORT_REGS_NS_DATA          (READSECUREPORT + 0x3)
#define READSECUREPORT_REGS_NS_DATA_BYTES    0x1


// Set the macro 'TXNONSECUREPORT' to the base of port 'txNonSecurePort'
#ifndef TXNONSECUREPORT
    #error TXNONSECUREPORT is undefined.It needs to be set to the port base address
#endif
#define TXNONSECUREPORT_TXREGS_TX_REQ    (TXNONSECUREPORT + 0x0)

#define TXNONSECUREPORT_TXREGS_TX_ACK    (TXNONSECUREPORT + 0x1)

#define TXNONSECUREPORT_TXREGS_DATATXLOCALHEADER    (TXNONSECUREPORT + 0x2)

#define TXNONSECUREPORT_TXREGS_DATATXLOCAL    (TXNONSECUREPORT + 0x4)


// Set the macro 'TXNONSECUREPORT2' to the base of port 'txNonSecurePort2'
#ifndef TXNONSECUREPORT2
    #error TXNONSECUREPORT2 is undefined.It needs to be set to the port base address
#endif
#define TXNONSECUREPORT2_TXREGS_TX_REQ    (TXNONSECUREPORT2 + 0x0)

#define TXNONSECUREPORT2_TXREGS_TX_ACK    (TXNONSECUREPORT2 + 0x1)

#define TXNONSECUREPORT2_TXREGS_DATATXLOCALHEADER    (TXNONSECUREPORT2 + 0x2)

#define TXNONSECUREPORT2_TXREGS_DATATXLOCAL    (TXNONSECUREPORT2 + 0x4)



#endif