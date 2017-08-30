/*
Navicat MySQL Data Transfer

Source Server         : 192.168.139.210
Source Server Version : 50528
Source Host           : 192.168.139.210:3306
Source Database       : sklCloudTest

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2015-06-04 15:46:30
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `t_platform_p2p`
-- ----------------------------
DROP TABLE IF EXISTS `t_platform_p2p`;
CREATE TABLE `t_platform_p2p` (
  `camera_serialno` varchar(32) NOT NULL COMMENT 'IP摄像头的序列号',
  `local_camera_ip` varchar(32) NOT NULL DEFAULT '' COMMENT 'IP摄像头的内网地址',
  `mapping_camera_ip` varchar(32) NOT NULL DEFAULT '' COMMENT 'IP摄像头的外网地址',
  `nat_type` tinyint(1) unsigned NOT NULL DEFAULT '9' COMMENT 'IP摄像头的NAT类型，0 - Full Cone NAT 1 -  Restricted Cone NAT 2 - Port Restricted Cone NAT 3 - Symmetric NAT',
  `msg_type` tinyint(1) unsigned NOT NULL COMMENT '消息类型 0 - RTSP 1 - HTTP',
  `createtime` datetime DEFAULT NULL COMMENT '信息生成时间',
  `modifytime` datetime DEFAULT NULL COMMENT '信息更新时间',
  PRIMARY KEY (`camera_serialno`),
  CONSTRAINT `t_platform_p2p_ibfk_1` FOREIGN KEY (`camera_serialno`) REFERENCES `t_platform_ipcamera` (`camera_serialno`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='P2P信息表';

