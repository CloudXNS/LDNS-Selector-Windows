### Linux 用法
* 打开终端，并进入源代码的 DNSPing 目录内
* 先运行 cmake . 生成 Makefile 文件，再运行 make 进行编译
* 所有编译完成后当前目录内将出现编译好的二进制文件


### 用法
       DNSPing [-options] domain target
  e.g. DNSPing -a -qt AAAA -n 5 -w 500 -edns0 www.google.com 8.8.4.4


### 选项
   ?/-h              打印说明
   -t                直到按下 Control-Break 或 Control-C 才停止 Ping
                     想看从 Ping 开始到按下时的统计信息但不想停止请按 Control-Break
                     想停止 Ping 并查看统计信息请按 Control-C
   -a                反向解析地址的域名
   -n count          发送 Ping 的数量
                     Count 必须介乎于 1 - 0xFFFF/65535
   -f                设定 Ping 数据包的不分片选项（只适用于IPv4）
                     不适用于 Linux 平台
   -i hoplimit/ttl   设定 Ping 数据包的跳数限制/生存时间
                     HopLimit/TTL 必须介乎于 1 - 255
   -w timeout        设定超时时间（单位：毫秒）
                     Timeout 必须介乎于 500 - 0xFFFF/65535
   -id dns_id        设定 DNS 请求包头的 ID
                     DNS ID 必须介乎于 0x0001 - 0xFFFF/65535
   -qr               设定 DNS 请求包头的 QR 标志
   -opcode opcode    设定 DNS 请求包头的 OPCode
                     OPCode 必须介乎于 0x0000 - 0x00FF/255
   -aa               设定 DNS 请求包头的 AA 标志
   -tc               设定 DNS 请求包头的 TC 标志
   -rd               设定 DNS 请求包头的 RD 标志
   -ra               设定 DNS 请求包头的 RA 标志
   -ad               设定 DNS 请求包头的 AD 标志
   -cd               设定 DNS 请求包头的 CD 标志
   -rcode rcode      设定 DNS 请求包头的 RCode
                     RCode 必须介乎于 0x0000 - 0x00FF/255
   -qn count         设定 DNS 请求包头的 Question count
                     Question count 必须介乎于 0x0001 - 0xFFFF/65535
   -ann count        设定 DNS 请求包头的 Answer count
                     Answer count 必须介乎于 0x0001 - 0xFFFF/65535
   -aun count        设定 DNS 请求包头的 Authority count
                     Authority count 必须介乎于 0x0001 - 0xFFFF/65535
   -adn count        设定 DNS 请求包头的 Additional count
                     Additional count 必须介乎于 0x0001 - 0xFFFF/65535
   -ti interval_time 设定每次请求之间的时间间隔（单位：毫秒）
   -edns0            发送时添加 EDNS0 标签
   -payload length   设定 EDNS0 标签的 UDP Payload length
                     Payload length 必须介乎于 512 - 0xFFFF/65535
   -dnssec           发送时添加可以接受 DNSSEC 的请求
                     启用添加可以接受 DNSSEC 时发送时添加 EDNS0 标签也会被启用
   -qt Type          设定请求类型 Query Type
                     Query 类型的值必须介乎于 0x0001 - 0xFFFF/65535
                     Type: A|NS|MD|MF|CNAME|SOA|MB|MG|MR|NULL|WKS|PTR|HINFO|
                           MINFO|MX|TXT|RP|AFSDB|X25|ISDN|RT|NSAP|NSAPPTR|
                           SIG|KEY|PX|GPOS|AAAA|LOC|NXT|EID|NIMLOC|SRV|ATMA|
                           NAPTR|KX|A6|CERT|DNAME|SINK|OPT|APL|DS|SSHFP|
                           IPSECKEY|RRSIG|NSEC|DNSKEY|DHCID|NSEC3|NSEC3PARAM|
                           TLSA|HIP|NINFO|RKEY|TALINK|CDS|CDNSKEY|OPENPGPKEY|
                           SPF|UINFO|UID|GID|UNSPEC|NID|L32|L64|LP|EUI48|
                           EUI64|TKEY|TSIG|IXFR|AXFR|MAILB|MAILA|ANY|URI|
                           CAA|TA|DLV|RESERVED
   -qc classes       设定请求类型 Query Classes
                     Classes 类型的值必须介乎于 0x0001 - 0xFFFF/65535
                     Classes: IN|CSNET|CHAOS|HESIOD|NONE|ALL|ANY
   -p service_name   设定 UDP 端口或服务名称
                     UDP 端口的值必须介乎于 0x0001 - 0xFFFF/65535
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
   -rawdata raw_data 设定想要发送的原始数据
                     RAW_Data 是以十六进制表示的数据，不需要在前面加 0x
                     RAW_Data 长度必须介乎于 64 - 1500 字节
   -raw service_name 设定要使用的原始套接字类型
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
   -socks target            设定 SOCKS 服务器的地址
                            地址的格式是 地址:端口，例如 [::1]:1080
   -socks_username username 设定 SOCKS 服务器的用户名
                            用户名长度必须介乎于 1 - 255 字节
   -socks_password password 设定 SOCKS 服务器的密码
                            密码长度必须介乎于 1 - 255 字节
   -buf size                设定接收缓冲区长度
                            缓冲区长度必须介乎于 512 - 4096 字节
   -dv                      关闭数据包验证
   -show type               显示收到的解析包的内容
                            Type: Result|Hex
   -of file_name            输出结果到文本文件
                            文本文件名称长度必须小于 260 字节
   -6                       强制使用 IPv6
   -4                       强制使用 IPv4
   domain                   设定发送 DNS 请求用的查询的域名
   target                   Ping 的目标，支持 IPv4/IPv6 地址和域名


### TCPing
DNSPing 使用的是 UDP 协议，TCP 协议不需要特制的程序进行 Ping
直接使用 TCPing 对目标服务器的 53 端口进行 Ping 即可


### TraceTCP
直接使用 TraceTCP 对目标服务器的 53 端口进行路由追踪即可
