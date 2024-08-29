#ifndef PC_PROTOCOL_H
#define PC_PROTOCOL_H

#include "udp_protocol.h"
#include "uv_state.h"
#include "i_server_data.h"


namespace Pult {
class PC_Protocol: public QObject, public MetaUdpProtocol {
    Q_OBJECT
public:
explicit PC_Protocol(QHostAddress _receiverIP, int _receiverPort, QHostAddress _senderIP, \
                         int _senderPort, int freq, int selectAgent, QObject *parent = 0)
{
    udpProtocol = new UdpProtocol <ToPult, FromPult> (_receiverIP, _receiverPort, _senderIP, _senderPort, \
                                                    freq, selectAgent, parent);
    connect(timer,SIGNAL(timeout()),SLOT(sendData()));
    connect(udpProtocol->getReceiveSocket(),SIGNAL(readyRead()),SLOT(receiveData()));
    set_ip_receiver(udpProtocol->ip_receiver());
    set_ip_sender (udpProtocol->ip_sender());
    set_port_receiver(udpProtocol->port_receiver());
    set_port_sender (udpProtocol->port_sender());
    nmbAgent = selectAgent;
}
    int nmbAgent;
signals:
    void dataReceived();
public slots:
    //запуск обмена
    void startExchange(){
        timer->start(1000/udpProtocol->getFrequency());
    }
    //остановить обмен
    void stopExhange(){
        timer->stop();
    }
    void sendData(){
        send_data = uv_server.generateFullMessage(nmbAgent);
        udpProtocol->send_data = send_data;
        qDebug() << (int)(send_data.cSMode);
        udpProtocol->sendData();
    }

    void receiveData(){
        udpProtocol->receiveData();
        rec_data = udpProtocol->rec_data;
        uv_server.parseFullMessage(rec_data, nmbAgent);
        emit dataReceived();
    }
public:
    ToPult  rec_data;
    FromPult send_data;
    UdpProtocol <ToPult, FromPult> *udpProtocol;

    IServerData uv_server;

    bool bindState(){return udpProtocol->bindState();}
};
} //namespace Pult


#endif // PC_PROTOCOL_H
