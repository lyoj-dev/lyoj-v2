-- CREATE TABLE contest (id int(255) NULL,title longtext NULL,starttime int(20) NULL,duration int(20) NULL,type int(11) NULL,rated tinyint(1) NULL DEFAULT 0,lang varchar(1024) NULL);
-- CREATE TABLE contest_problem (pid int(255) NULL,id int(255) NULL);
-- CREATE TABLE contest_ranking (id int(255) NULL,uid int(255) NULL,score int(255) NULL,time int(255) NULL,info longtext NULL,penalty int(255) NULL);
-- CREATE TABLE contest_signup (id int(255) NULL,uid int(255) NULL);
-- CREATE TABLE crontab (id int(255) NULL,duration int(20) NULL,lasttime int(20) NULL,command longtext NULL,name longtext NULL);
-- CREATE TABLE judger (id varchar(128) NULL,config longtext NULL,name longtext NULL,heartbeat int(20) NULL);
-- CREATE TABLE logindata (uid int(255) NULL,csrf longtext NULL,sessdata longtext NULL,time int(20) NULL);
CREATE TABLE problem (
    id int,
    alias longtext,
    title longtext,
    bg longtext,
    descrip longtext,
    input longtext,
    output longtext,
    cases longtext,
    hint longtext,
    hidden boolean,
    banned boolean,
    difficulty int,
    tags longtext,
    uid int
);
CREATE TABLE submission (
    id int,
    uid int,
    pid int,
    lang int,
    code longtext,
    result longtext,
    time int,
    status int,
    score int,
    judged boolean,
    contest int
);
CREATE TABLE tags (
    id int,
    title longtext,
    type int
);
INSERT INTO tags VALUES (
    0,
    '',
    0
);
-- CREATE TABLE user (id int(255) NULL,name varchar(255) NULL,passwd longtext NULL,title varchar(255) NULL,permission int(10) NULL,email longtext NULL,salt longtext NULL,salttime int(20) NULL,verify tinyint(1) NULL,verify_code varchar(200) NULL,rating int(255) NULL,uptime int(20) NULL,banned tinyint(1) NULL,passwd_token varchar(1024) NULL);