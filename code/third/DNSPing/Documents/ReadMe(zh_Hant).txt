### Linux 用法
* 打開終端，並進入原始程式碼的 DNSPing 目錄內
* 先運行 cmake . 生成 Makefile 檔，再運行 make 進行編譯
* 所有編譯完成後目前的目錄內將出現編譯好的二進位檔案


### 用法
       DNSPing [-options] domain target
  e.g. DNSPing -a -qt AAAA -n 5 -w 500 -edns0 www.google.com 8.8.4.4


### 选项
   ?/-h              列印說明
   -t                直到按下 Control-Break 或 Control-C 才停止 Ping
                     想看從 Ping 開始到按下時的統計資訊但不想停止請按 Control-Break
                     想停止 Ping 並查看統計資訊請按 Control-C
   -a                反向解析位址的功能變數名稱
   -n count          發送 Ping 的數量
                     Count 必須介乎于 1 - 0xFFFF/65535
   -f                設定 Ping 資料包的不分片選項（只適用于IPv4）
                     不適用於 Linux 平台
   -i hoplimit/ttl   設定 Ping 資料包的跳數限制/存留時間
                     HopLimit/TTL 必須介乎于 1 - 255
   -w timeout        設定超時時間（單位：毫秒）
                     Timeout 必須介乎于 500 - 0xFFFF/65535
   -id dns_id        設定 DNS 請求包頭的 ID
                     DNS ID 必須介乎于 0x0001 - 0xFFFF/65535
   -qr               設定 DNS 請求包頭的 QR 標誌
   -opcode opcode    設定 DNS 請求包頭的 OPCode
                     OPCode 必須介乎于 0x0000 - 0x00FF/255
   -aa               設定 DNS 請求包頭的 AA 標誌
   -tc               設定 DNS 請求包頭的 TC 標誌
   -rd               設定 DNS 請求包頭的 RD 標誌
   -ra               設定 DNS 請求包頭的 RA 標誌
   -ad               設定 DNS 請求包頭的 AD 標誌
   -cd               設定 DNS 請求包頭的 CD 標誌
   -rcode rcode      設定 DNS 請求包頭的 RCode
                     RCode 必須介乎于 0x0000 - 0x00FF/255
   -qn count         設定 DNS 請求包頭的 Question count
                     Question count 必須介乎于 0x0001 - 0xFFFF/65535
   -ann count        設定 DNS 請求包頭的 Answer count
                     Answer count 必須介乎于 0x0001 - 0xFFFF/65535
   -aun count        設定 DNS 請求包頭的 Authority count
                     Authority count 必須介乎于 0x0001 - 0xFFFF/65535
   -adn count        設定 DNS 請求包頭的 Additional count
                     Additional count 必須介乎于 0x0001 - 0xFFFF/65535
   -ti interval_time 設定每次請求之間的時間間隔（單位：毫秒）
   -edns0            發送時添加 EDNS0 標籤
   -payload length   設定 EDNS0 標籤的 UDP Payload length
                     Payload length 必須介乎于 512 - 0xFFFF/65535
   -dnssec           發送時添加可以接受 DNSSEC 的請求
                     啟用添加可以接受 DNSSEC 時發送時添加 EDNS0 標籤也會被啟用
   -qt type          設定請求類型 Query Type
                     Query 類型的值必須介乎于 0x0001 - 0xFFFF/65535
                     Type: A|NS|MD|MF|CNAME|SOA|MB|MG|MR|NULL|WKS|PTR|HINFO|
                           MINFO|MX|TXT|RP|AFSDB|X25|ISDN|RT|NSAP|NSAPPTR|
                           SIG|KEY|PX|GPOS|AAAA|LOC|NXT|EID|NIMLOC|SRV|ATMA|
                           NAPTR|KX|A6|CERT|DNAME|SINK|OPT|APL|DS|SSHFP|
                           IPSECKEY|RRSIG|NSEC|DNSKEY|DHCID|NSEC3|NSEC3PARAM|
                           TLSA|HIP|NINFO|RKEY|TALINK|CDS|CDNSKEY|OPENPGPKEY|
                           SPF|UINFO|UID|GID|UNSPEC|NID|L32|L64|LP|EUI48|
                           EUI64|TKEY|TSIG|IXFR|AXFR|MAILB|MAILA|ANY|URI|
                           CAA|TA|DLV|RESERVED
   -qc classes       設定請求類型 Query Classes
                     Classes 類型的值必須介乎于 0x0001 - 0xFFFF/65535
                     Classes: IN|CSNET|CHAOS|HESIOD|NONE|ALL|ANY
   -p service_name   設定 UDP 埠
                     UDP 埠的值必須介乎于 0x0001 - 0xFFFF/65535
                     Protocol: TCPMUX|ECHO|DISCARD|SYSTAT|DAYTIME|NETSTAT|
                               QOTD|MSP|CHARGEN|FTP|SSH|TELNET|SMTP|
                               TIME|RAP|RLP|NAME|WHOIS|TACACS|DNS|XNSAUTH|MTP
                               BOOTPS|BOOTPC|TFTP|RJE|FINGER|TTYLINK|SUPDUP|
                               SUNRPC|SQL|NTP|EPMAP|NETBIOSNS|NETBIOSDGM|
                               NETBIOSSSN|IMAP|BFTP|SGMP|SQLSRV|DMSP|SNMP|
                               SNMPTRAP|ATRTMP|ATHBP|QMTP|IPX|IMAP|IMAP3|
                               BGMP|TSP|IMMP|ODMR|RPC2PORTMAP|CLEARCASE|
                               HPALARMMGR|ARNS|AURP|LDAP|UPS|SLP|SNPP|
                               MICROSOFTDS|KPASSWD|TCPNETHASPSRV|RETROSPECT|
                               ISAKMP|BIFFUDP|WHOSERVER|SYSLOG|ROUTERSERVER|
                               NCP|COURIER|COMMERCE|RTSP|NNTP|HTTPRPCEPMAP|
                               IPP|LDAPS|MSDP|AODV|FTPSDATA|FTPS|NAS|TELNETS
   -rawdata raw_data 設定發送原始資料
                     原始資料為不需要添加 0x 開頭的16進制數
                     原始資料長度必須介乎于 64 - 1500 位元組
   -raw service_name 設定 RAW 通訊端類型
                     Service Name: HOPOPTS|ICMP|IGMP|GGP|IPV4|ST|TCP|CBT|EGP|
                                   IGP|BBNRCCMON|NVPII|PUP|ARGUS|EMCON|XNET|
                                   CHAOS|MUX|DCN|HMP|PRM|IDP|TRUNK_1|TRUNK_2
                                   LEAF_1|LEAF_2|RDP|IRTP|ISOTP4|MFE|MERIT|
                                   DCCP|3PC|IDPR|XTP|DDP|IDPRCMTP|TP++|IL|
                                   IPV6|SDRP|ROUTING|FRAGMENT|IDRP|RSVP|GRE|
                                   DSR|BNA|ESP|AH|NLSP|SWIPE|NARP|MOBILE|TLSP
                                   SKIP|ICMPV6|NONE|DSTOPTS|AHI|CFTP|ALN|SAT|
                                   KRYPTOLAN|RVD|IPPC|ADF|SATMON|VISA|IPCV|
                                   CPNX|CPHB|WSN|PVP|BR|ND|ICLFXBM|WBEXPAK|
                                   ISO|VMTP|SVMTP|VINES|TTP|IPTM|NSFNET|DGP|
                                   TCF|EIGRP|SPRITE|LARP|MTP|AX25|IPIP|MICP|
                                   SCC|ETHERIP|ENCAP|APES|GMTP|IFMP|PNNI|PIM|
                                   ARIS|SCPS|QNX|AN|IPCOMP|SNP|COMPAQ|IPX|PGM
                                   0HOP|L2TP|DDX|IATP|STP|SRP|UTI|SMP|SM|
                                   PTP|ISIS|FIRE|CRTP|CRUDP|SSCOPMCE|IPLT|
                                   SPS|PIPE|SCTP|FC|RSVPE2E|MOBILITY|UDPLITE|
                                   MPLS|MANET|HIP|SHIM6|WESP|ROHC|TEST-1|
                                   TEST-2|RAW
   -socks target                   設定 SOCKS 伺服器的位址
                                   位址的格式是 位址:埠，例如 [::1]:1080
   -socks_username username        設定 SOCKS 伺服器的使用者名
                                   使用者名長度必須介乎于 1 - 255 位元組
   -socks_password password        設定 SOCKS 伺服器的密碼
                                   密碼長度必須介乎于 1 - 255 位元組
   -buf size                       設定接收緩衝區長度
                                   緩衝區長度必須介乎于 512 - 4096 位元組
   -dv                             關閉資料包驗證
   -show type                      顯示收到的解析包的內容
                                   Type: Result|Hex
   -of file_name                   輸出結果到文字檔
                                   文字檔名稱長度必須小於 260 位元組
   -6                              強制使用 IPv6
   -4                              強制使用 IPv4
   domain                          設定發送 DNS 請求用的查詢的功能變數名稱
   target                          Ping 的目標，支援 IPv4/IPv6 位址和功能變數名稱


### TCPing
DNSPing 使用的是 UDP 協定，TCP 協定不需要特製的程式進行 Ping
直接使用 TCPing 對目標伺服器的 53 埠進行 Ping 即可


### TraceTCP
直接使用 TraceTCP 對目標伺服器的 53 埠進行路由追蹤即可
