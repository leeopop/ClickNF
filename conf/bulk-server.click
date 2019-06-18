require(library test-tcp-layer2.click)

define($DEV0 0, $ADDR0 10.0.20.2, $MAC0 3c:fd:fe:9e:5c:88)
AddressInfo($DEV0 $ADDR0 $MAC0);

tcp_layer :: TCPLayer(ADDRS $ADDR0, VERBOSE false);
tcp_bulks :: TCPBulkServer($ADDR0, 9000, BUFLEN 2048, VERBOSE false);

tcp_bulks[0] -> [1]tcp_layer;
tcp_layer[1] -> [0]tcp_bulks;

dpdk0 :: DPDK($DEV0, BURST 32, TX_RING_SIZE 512, RX_RING_SIZE 512, TX_IP_CHECKSUM 1, TX_TCP_CHECKSUM 1, RX_CHECKSUM 1, RX_STRIP_CRC 1);

//arpr :: ARPResponder($DEV0);
//arpq :: ARPQuerier($DEV0, SHAREDPKT true);
arps :: FixedArp(3c:fd:fe:9e:5c:88, 3c:fd:fe:a4:d5:c8);

//arpq[0] -> dpdk0;
//arpq[1] -> dpdk0;
arps[0] -> dpdk0;

tcp_layer[0]
  -> GetIPAddress(16)  // This only works with nodes in the same network
  //-> [0]arpq;
  -> [0]arps;

dpdk0
  -> HostEtherFilter($DEV0)
  -> class :: FastClassifier(12/0806 20/0001, // ARP query
                             12/0806 20/0002, // ARP response
                             12/0800);        // IP
     //class[0] -> [0]arpr -> dpdk0;
     //class[1] -> [1]arpq;
     class[0] -> Discard;
     class[1] -> Discard;
     class[2] -> Strip(14)
              -> CheckIPHeader(CHECKSUM false)
              -> IPClassifier(tcp dst host $ADDR0)
              -> CheckTCPHeader(CHECKSUM false)
              -> [0]tcp_layer;
