-- CREATE TABLE contest (id int(255) NULL,title longtext NULL,starttime int(20) NULL,duration int(20) NULL,type int(11) NULL,rated tinyint(1) NULL DEFAULT 0,lang varchar(1024) NULL);
-- CREATE TABLE contest_problem (pid int(255) NULL,id int(255) NULL);
-- CREATE TABLE contest_ranking (id int(255) NULL,uid int(255) NULL,score int(255) NULL,time int(255) NULL,info longtext NULL,penalty int(255) NULL);
-- CREATE TABLE contest_signup (id int(255) NULL,uid int(255) NULL);
-- CREATE TABLE crontab (id int(255) NULL,duration int(20) NULL,lasttime int(20) NULL,command longtext NULL,name longtext NULL);
-- CREATE TABLE judger (id varchar(128) NULL,config longtext NULL,name longtext NULL,heartbeat int(20) NULL);
CREATE TABLE session (
    uid int,
    session longtext,
    time int
);
CREATE TABLE problem (
    id int,
    alias text,
    title text,
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
    uid int,
    groups longtext,
    langs longtext
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
    title text,
    type int
);
CREATE TABLE userGroup (
    id int,
    title text,
    description longtext,
    permission int
);
DELETE FROM userGroup WHERE id = 1 OR id = 2;
INSERT INTO userGroup VALUES (
    1,
    'Default',
    'Default group for all users',
    0
);
INSERT INTO userGroup VALUES (
    2,
    'Admin',
    'Administrators',
    0
);
CREATE TABLE user (
    id int,
    title text,
    realname text,
    schoolId text,
    institude text,
    password text,
    createTime int,
    rating int,
    groups longtext
);
DELETE FROM user WHERE id = 0;
INSERT INTO user VALUES (
    0,
    'Anonymous',
    '',
    '',
    '',
    '',
    0,
    0,
    '[1]'
);