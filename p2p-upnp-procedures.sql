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
-- -----------------------------
-- `p2p-upnp` procedure group 
-- -----------------------------


-- ----------------------------------------------------
-- `p2p-upnp` procedure checkID for `t_platform_ipcamera`
-- ----------------------------------------------------
CREATE DEFINER=`root`@`%` PROCEDURE `checkID`(IN `sn` varchar(32),IN `random` varchar(32))
    COMMENT 'checkID-验证sn与random组合'
BEGIN
	select COUNT(*) from t_platform_ipcamera  
	where camera_serialno = sn and camera_send_ipc_random = random limit 1;
END

-- ----------------------------------------------------
-- `p2p-upnp` procedure regupnpinfo for `t_platform_p2p` and `t_platform_mapping`
-- ----------------------------------------------------
CREATE DEFINER=`root`@`%` PROCEDURE `regupnpinfo`(IN `sn` varchar(32),IN `local_ip` varchar(32),IN `map_ip` varchar(32),IN `msg_type` tinyint)
    COMMENT 'regupnpinfo-注册p2p(ip)表信息'
BEGIN
	#SET NAMES utf8;
	delete from t_platform_mapping where camera_serialno = sn LIMIT 5;
	DELETE from t_platform_p2p where camera_serialno = sn LIMIT 1;
	insert into t_platform_p2p(camera_serialno, local_camera_ip, 
					mapping_camera_ip, msg_type, createtime, modifytime)
	 				values( sn, local_ip, map_ip, msg_type, now(), now());	
END

-- ----------------------------------------------------
-- `p2p-upnp` procedure regupnpport for `t_platform_mapping`
-- ----------------------------------------------------
CREATE DEFINER=`root`@`%` PROCEDURE `regupnpport`(IN `sn` varchar(32),IN `local_port` int,IN `map_port` int,IN `socket_type` varchar(16),IN `port_type` varchar(16))
    COMMENT 'regupnpport-注册mapping端口表信息'
BEGIN
	#SET NAMES utf8;
	insert into t_platform_mapping(camera_serialno, 
					local_port, mapping_port, port_type, socket_type) 
					values( sn, local_port, map_port, socket_type, port_type);
END

-- ----------------------------------------------------
-- `p2p-upnp` procedure freshupnpinfo for `t_platform_ipcamera`, `t_platform_p2p` and `t_platform_mapping` 
-- ----------------------------------------------------
CREATE DEFINER=`root`@`%` PROCEDURE `freshupnpinfo`(IN `sn` varchar(32),IN `random` varchar(32),IN `local_ip` varchar(32),IN `map_ip` varchar(32),IN `msg_type` tinyint,IN `http_lp` int,IN `http_mp` int,IN `http_st` varchar(16),IN `http_pt` varchar(16),IN `rtsp_lp` int,IN `rtsp_mp` int,IN `rtsp_st` varchar(16),IN `rtsp_pt` varchar(16),IN `rtp_lp` int,IN `rtp_mp` int,IN `rtp_st` varchar(16),IN `rtp_pt` varchar(16))
    COMMENT 'freshupnpinfo-融合版本的upnp SQL语句'
BEGIN
	DECLARE flag INT DEFAULT 0;
	SET NAMES utf8;
	select COUNT(*) into flag from t_platform_ipcamera  where camera_serialno = `sn`  and camera_send_ipc_random = `random`  limit 1;
	if flag = 1 THEN
	delete from t_platform_mapping where camera_serialno = sn LIMIT 3;
	DELETE from t_platform_p2p where camera_serialno = sn LIMIT 1;
	insert into t_platform_p2p(camera_serialno, local_camera_ip, 
					mapping_camera_ip, msg_type, createtime, modifytime)
	 				values( `sn` , `local_ip` , `map_ip` , `msg_type` , now(), now());
		if(`http_lp` != 0 AND `http_mp` != 0 ) THEN
		insert into t_platform_mapping(camera_serialno, 
						local_port, mapping_port, port_type, socket_type) 
						values( `sn`, `http_lp` , `http_mp` , `http_st` , `http_pt` );	
		end if;
		if(`rtsp_lp` != 0 AND `rtsp_mp` != 0 ) THEN
		insert into t_platform_mapping(camera_serialno, 
						local_port, mapping_port, port_type, socket_type) 
						values( `sn`, `rtsp_lp` , `rtsp_mp` , `rtsp_st` , `rtsp_pt` );
		end if;
		if(`rtp_lp` != 0 AND `rtp_mp` != 0 ) THEN
		insert into t_platform_mapping(camera_serialno, 
						local_port, mapping_port, port_type, socket_type) 
						values( `sn`, `rtp_lp` , `rtp_mp` , `rtp_st` , `rtp_pt` );
		end if;
	end if;
END

