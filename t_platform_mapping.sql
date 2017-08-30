/*
Navicat MySQL Data Transfer

Source Server         : 192.168.139.210
Source Server Version : 50528
Source Host           : 192.168.139.210:3306
Source Database       : sklCloudTest

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2015-06-04 15:46:40
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `t_platform_mapping`
-- ----------------------------
DROP TABLE IF EXISTS `t_platform_mapping`;
CREATE TABLE `t_platform_mapping` (
  `camera_serialno` varchar(32) NOT NULL COMMENT 'IP摄像头的序列号',
  `local_port` int(8) unsigned NOT NULL COMMENT 'IP摄像头的内部端口',
  `mapping_port` int(8) unsigned NOT NULL COMMENT 'IP摄像头的NAT映射端口',
  `socket_type` varchar(16) NOT NULL COMMENT 'SOCKET类型 0 - TCP 1 - UDP 2 - HTTP',
  `port_type` varchar(16) NOT NULL COMMENT 'NAT映射端口的业务类型',
  PRIMARY KEY (`camera_serialno`,`local_port`),
  CONSTRAINT `t_platform_mapping_ibfk_1` FOREIGN KEY (`camera_serialno`) REFERENCES `t_platform_p2p` (`camera_serialno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='端口映射表';

