CREATE TABLE contest (
    id int,
    title longtext,
    starttime int,
    duration int,
    type int,
    rated boolean,
    tags longtext,
    problems longtext,
    password text,
    info longtext,
    uid int
);
CREATE TABLE contest_ranking (
    id int,
    uid int,
    score int,
    time int,
    time2 int,
    info longtext,
    penalty int
);
CREATE TABLE contest_signup (
    id int,
    uid int
);
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
    393
);
INSERT INTO userGroup VALUES (
    2,
    'Admin',
    'Administrators',
    2047
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
    groups longtext,
    info longtext
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
    '[1]',
    ''
);