CREATE DATABASE IF NOT EXISTS im_db DEFAULT CHARSET utf8mb4;
USE im_db;

-- 用户表
CREATE TABLE IF NOT EXISTS `user` (
  `id` INT PRIMARY KEY AUTO_INCREMENT COMMENT '用户ID',
  `username` VARCHAR(32) NOT NULL UNIQUE COMMENT '用户名',
  `password` VARCHAR(32) NOT NULL COMMENT '密码(MD5)',
  `status` TINYINT DEFAULT 0 COMMENT '在线状态 0离线 1在线',
  `uid` INT NOT NULL UNIQUE COMMENT '用户UID'
) COMMENT '用户表';

-- 离线消息表
CREATE TABLE IF NOT EXISTS `offline_msg` (
  `id` INT PRIMARY KEY AUTO_INCREMENT COMMENT '消息ID',
  `recv_user` VARCHAR(32) COMMENT '接收方',
  `send_user` VARCHAR(32) COMMENT '发送方',
  `msg` TEXT COMMENT '消息',
  `create_time` DATETIME DEFAULT NOW() COMMENT '创建时间'
) COMMENT '离线消息';
