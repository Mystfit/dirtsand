/******************************************************************************
 * This file is part of dirtsand.                                             *
 *                                                                            *
 * dirtsand is free software: you can redistribute it and/or modify           *
 * it under the terms of the GNU General Public License as published by       *
 * the Free Software Foundation, either version 3 of the License, or          *
 * (at your option) any later version.                                        *
 *                                                                            *
 * dirtsand is distributed in the hope that it will be useful,                *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with dirtsand.  If not, see <http://www.gnu.org/licenses/>.          *
 ******************************************************************************/

#include "Lobby.h"
#include "GateKeeper/GateServ.h"
#include "FileServ/FileServer.h"
#include "AuthServ/AuthServer.h"
#include "Types/Uuid.h"
#include "SockIO.h"
#include "errors.h"
#include <pthread.h>
#include <cstdio>

enum ConnType
{
    e_ConnCliToAuth = 10,
    e_ConnCliToGame = 11,
    e_ConnCliToFile = 16,
    e_ConnCliToCsr = 20,
    e_ConnCliToGateKeeper = 22,
};

struct ConnectionHeader
{
    uint8_t m_connType;
    uint16_t m_sockHeaderSize;
    uint32_t m_buildId, m_buildType, m_branchId;
    DS::Uuid m_productId;
};

static pthread_t s_lobbyThread;
static DS::SocketHandle s_listenSock;

void* dm_lobby(void*)
{
    printf("Lobby running on port 14617\n");
    try {
        for ( ;; ) {
            DS::SocketHandle client = DS::AcceptSock(s_listenSock);
            if (!client)
                break;

            ConnectionHeader header;
            header.m_connType = DS::RecvValue<uint8_t>(client);
            header.m_sockHeaderSize = DS::RecvValue<uint16_t>(client);
            header.m_buildId = DS::RecvValue<uint32_t>(client);
            header.m_buildType = DS::RecvValue<uint32_t>(client);
            header.m_branchId = DS::RecvValue<uint32_t>(client);
            DS::RecvBuffer(client, header.m_productId.m_bytes,
                           sizeof(header.m_productId.m_bytes));

            switch (header.m_connType) {
            case e_ConnCliToGateKeeper:
                DS::GateKeeper_Add(client);
                break;
            case e_ConnCliToFile:
                DS::FileServer_Add(client);
                break;
            case e_ConnCliToAuth:
                DS::AuthServer_Add(client);
                break;
            case e_ConnCliToGame:
                fprintf(stderr, "[%s] Unhandled game server connection\n",
                        DS::SockIpAddress(client).c_str());
                DS::FreeSock(client);
                break;
            case e_ConnCliToCsr:
                printf("[Lobby] %s - CSR client?  Get that mutha outta here!\n",
                       DS::SockIpAddress(client).c_str());
                DS::FreeSock(client);
                break;
            default:
                printf("[Lobby] %s - Unknown connection type!  Abandon ship!\n",
                       DS::SockIpAddress(client).c_str());
                DS::FreeSock(client);
                break;
            }
        }
    } catch (DS::AssertException ex) {
        fprintf(stderr, "[Lobby] Assertion failed at %s:%ld:  %s\n",
                ex.m_file, ex.m_line, ex.m_cond);
    }

    DS::FreeSock(s_listenSock);
    return 0;
}

void DS::StartLobby()
{
    s_listenSock = DS::BindSocket(0, "14617");
    DS::ListenSock(s_listenSock);
    pthread_create(&s_lobbyThread, 0, &dm_lobby, 0);
}

void DS::StopLobby()
{
    DS::CloseSock(s_listenSock);
    pthread_join(s_lobbyThread, 0);
}