# 创建数据库
CREATE DATABASE IF NOT EXISTS `ELSFK` DEFAULT CHARSET utf8 COLLATE utf8_general_ci;

# 
USE `ELSFK`;

# 创建表  用户账号信息
CREATE TABLE IF NOT EXISTS `elsfk_users`(
    `u_id` INT UNSIGNED,
    `u_name` VARCHAR(24) NOT NULL,
    `u_passwd` CHAR(32) NOT NULL,
    `u_pw_old` CHAR(32),  # 之前的密码
    `last_re_pw` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,  # 不需要，可以删掉
    PRIMARY KEY ( `u_id` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

# 创建表  登陆记录  记录时间戳  地址
CREATE TABLE IF NOT EXISTS `elsfk_login`(
    `log_no` INT UNSIGNED AUTO_INCREMENT,   # 记录的编号
    `login_time` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    `logout_time` TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    `u_id` INT UNSIGNED NOT NULL,
    `u_ipv4` VARCHAR(15),
    `u_ipv6` VARCHAR(45),
    PRIMARY KEY ( `log_no` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

# 四人对战记录  玩家们的分数、存活时间
CREATE TABLE IF NOT EXISTS `elsfk_game_4`(
    `rec_no` INT UNSIGNED AUTO_INCREMENT,
    `rec_time_start` TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    `g_level` INT NOT NULL,
    `u_id1` INT UNSIGNED NOT NULL,
    `u_id2` INT UNSIGNED NOT NULL,
    `u_id3` INT UNSIGNED NOT NULL,
    `u_id4` INT UNSIGNED NOT NULL,
    `u_live1` INT UNSIGNED,
    `u_live2` INT UNSIGNED,
    `u_live3` INT UNSIGNED,
    `u_live4` INT UNSIGNED,
    `u_score1` INT UNSIGNED,
    `u_score2` INT UNSIGNED,
    `u_score3` INT UNSIGNED,
    `u_score4` INT UNSIGNED,
    PRIMARY KEY ( `rec_no` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;

# 玩家游戏信息
CREATE TABLE IF NOT EXISTS `elsfk_ug`(
    `u_id` INT UNSIGNED,
    `c_all` INT,        # 总场次
    `cc_4` INT,         # 4人对战场次
    `cc_41` INT,        # 4人对战第一名场次
    `cc_42` INT,        # 4人对战第二名场次
    `cc_43` INT,        # 4人对战第三名场次
    `cc_44` INT,        # 4人对战第四名场次
    `ca_41` INT,        # 最能苟活场次
    `ca_44` INT,        # 最速战速决场次
    `cc_ai` INT,        # ai对战场次
    `cw_ai` INT,        # ai对战赢场次
    `t_all` INT,        # 总游戏时长
    `t_live` INT,       # 总存活时长
    `win_str` INT,      # 当前连胜  4人对战积分前两名记为胜
    `win_str_m` INT,    # 最高连胜
    PRIMARY KEY ( `u_id` )
)ENGINE=InnoDB DEFAULT CHARSET=utf8;