/*
Navicat MySQL Data Transfer

Source Server         : 192.168.139.210
Source Server Version : 50528
Source Host           : 192.168.139.210:3306
Source Database       : sklCloudTest

Target Server Type    : MYSQL
Target Server Version : 50528
File Encoding         : 65001

Date: 2015-06-04 15:46:46
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `t_platform_ipcamera`
-- ----------------------------
DROP TABLE IF EXISTS `t_platform_ipcamera`;
CREATE TABLE `t_platform_ipcamera` (
  `camera_id` bigint(16) unsigned NOT NULL AUTO_INCREMENT COMMENT '摄像头编号，主键，无任何业务意义',
  `camera_serialno` varchar(32) NOT NULL COMMENT '摄像头序列号',
  `camera_mac` varchar(32) NOT NULL COMMENT '摄像头的MAC地址',
  `camera_nickname` varchar(64) DEFAULT NULL COMMENT '摄像头的昵称',
  `camera_style` varchar(32) DEFAULT NULL COMMENT '摄像头类型，如：P2P、CLOUD',
  `camera_model` varchar(16) NOT NULL COMMENT '摄像头型号，4个10进制数字，前2个表示系列，后两个表示分型，如：0001-0005表示LFM01-lFM05 0103表示HPC03',
  `camera_pincode` varchar(32) NOT NULL COMMENT '摄像头验证码',
  `camera_makedate` datetime NOT NULL COMMENT '摄像头生产日期',
  `camera_version` varchar(16) NOT NULL COMMENT '摄像头软版本号',
  `camera_random` varchar(32) DEFAULT NULL COMMENT '摄像头随机数',
  `camera_rece_ipc_random` varchar(32) DEFAULT NULL COMMENT '摄像头随机数, 收到IPC的 ',
  `camera_send_ipc_random` varchar(32) DEFAULT NULL COMMENT '云端随机数,   发送给IPC的 ',
  `camera_p2p_random` varchar(32) DEFAULT NULL COMMENT '云端P2P随机数,App读取p2p信息时获得',
  `camera_status` tinyint(1) unsigned DEFAULT '0' COMMENT '摄像头状态 0 - 未激活 1 - 已激活',
  `camera_validate` datetime DEFAULT NULL COMMENT '摄像头认证日期',
  `camera_expired` datetime DEFAULT NULL COMMENT '摄像头失效日期',
  `lenth` varchar(16) DEFAULT NULL,
  `width` varchar(16) DEFAULT NULL,
  PRIMARY KEY (`camera_id`),
  UNIQUE KEY `camera_mac` (`camera_mac`),
  UNIQUE KEY `camera_pincode` (`camera_pincode`),
  UNIQUE KEY `camera_serialno` (`camera_serialno`)
) ENGINE=InnoDB AUTO_INCREMENT=56 DEFAULT CHARSET=utf8 ROW_FORMAT=COMPACT COMMENT='摄像头表';


