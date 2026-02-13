<script lang="ts">
import { config } from '@/config';
import { loginInfo, myFetch, showMsg } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import markdownit from 'markdown-it';
import hljs from 'highlight.js';
import markdownItKatex from '@vscode/markdown-it-katex';
import MonacoEditor from 'monaco-editor-vue3';
import { i18n } from '@/i18n';
import crypto from 'crypto-js';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var json = await myFetch(config.apiBase + '/users/' + to.params.id);
    var edit = to.query.edit != undefined;

    next((e: any) => e.loading({
        data: json,
        edit: edit
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const t = i18n.global.t;
const loaded = ref(false);
const item: any = ref({});
const currPage = ref("info");
const isPreview = ref(false);
const username = ref("");
const info = ref("");
const enableBtn = ref(true);
const enableUploadBtn1 = ref(true);
const enableUploadBtn2 = ref(true);
const password1 = ref("");
const password2 = ref("");
const password3 = ref("");
const headerSrc = ref("");
const bannerSrc = ref("");

function loading(data: any) {
    if (data.edit) currPage.value = "edit";
    item.value = data.data;
    username.value = data.data.item.title;
    info.value = data.data.item.info;
    headerSrc.value = config.apiBase + '/users/' + item.value.item.id + '/header?t=' + new Date().getTime();
    bannerSrc.value = config.apiBase + '/users/' + item.value.item.id + '/banner?t=' + new Date().getTime();
    loaded.value = true;
}

defineExpose({ loading });

const md = markdownit({
    html: true,
    highlight: function (str: string, lang: string): string {
        if (str[str.length - 1] == '\n') str = str.substr(0, str.length - 1);
        if (lang && hljs.getLanguage(lang)) {
            try {
                var code: string = '<pre class="hljs"><code><ul>';
                var strs: string[] = str.split('\n');
                for (var i = 0; i < strs.length; i++) 
                    code += '<li class="hljs language-' + lang + '">' + hljs.highlight(lang, strs[i], true).value + '</li>'
                code += '</ul></code></pre>';
                return code;
            } catch (__) {}
        }
        var code: string = '<pre class="hljs"><code><ul>';
        var strs: string[] = str.split('\n');
        for (var i = 0; i < strs.length; i++) 
            code += '<li class="hljs language-' + lang + '">' + md.utils.escapeHtml(strs[i]) + '</li>'
        code += '</ul></code></pre>';
        return code;
    }
});
md.use(markdownItKatex);

async function uploadHeader() {
    var a = document.createElement('input');
    a.type = 'file';
    a.accept = 'image/jpg';
    a.onchange = async function() {
        enableUploadBtn1.value = false;
        var reader = new FileReader();
        reader.onload = async function() {
            var data = (this.result as string).split(',')[1];
            var res = await myFetch(config.apiBase + '/users/' + item.value.item.id + '/header/update', {
                method: 'POST',
                body: data
            }, false);
            if (res.code != 200) {
                showMsg("error", t('pages.users.uploadHeaderFailed'));
                enableUploadBtn1.value = true;
                return;
            }
            showMsg("success", t('pages.users.uploadHeaderSuccess'));
            headerSrc.value = config.apiBase + '/users/' + item.value.item.id + '/header?t=' + new Date().getTime();
            enableUploadBtn1.value = true;
        };
        if (a.files != null) reader.readAsDataURL(a.files[0]);
    };
    a.click();
}

async function uploadBanner() {
    var a = document.createElement('input');
    a.type = 'file';
    a.accept = 'image/jpg';
    a.onchange = async function() {
        enableUploadBtn2.value = false;
        var reader = new FileReader();
        reader.onload = async function() {
            var data = (this.result as string).split(',')[1];
            var res = await myFetch(config.apiBase + '/users/' + item.value.item.id + '/banner/update', {
                method: 'POST',
                body: data
            }, false);
            if (res.code != 200) {
                showMsg("error", t('pages.users.uploadBannerFailed'));
                enableUploadBtn2.value = true;
                return;
            }
            showMsg("success", t('pages.users.uploadBannerSuccess'));
            bannerSrc.value = config.apiBase + '/users/' + item.value.item.id + '/banner?t=' + new Date().getTime();
            enableUploadBtn2.value = true;
        };
        if (a.files != null) reader.readAsDataURL(a.files[0]);
    };
    a.click();
}

async function submit() {
    if (username.value == "") {
        showMsg("error", t('pages.users.submitFailedTitleEmpty'));
        return;
    }

    var data = {
        title: username.value,
        info: info.value
    };

    var res = await myFetch(config.apiBase + '/users/' + item.value.item.id + '/edit', {
        method: 'POST',
        body: JSON.stringify(data)
    }, false);

    if (res.code == 409) {
        showMsg("error", t('pages.users.submitFailedTitleConflict'));
        return;
    }
    if (res.code != 200) {
        showMsg("error", t('pages.users.submitFailed'));
        return;
    }
    showMsg("success", t('pages.users.submitSuccess'));
    item.value.item.title = username.value;
    loginInfo.value.title = username.value;
    item.value.item.info = info.value;
}

async function submitPassword() {
    if (password2.value != password3.value) {
        showMsg("error", t('pages.users.passwordNotMatch'));
        return;
    }

    var data = {
        oldPassword: password1.value == "" ? "" : crypto.MD5(password1.value).toString(),
        newPassword: password2.value == "" ? "" : crypto.MD5(password2.value).toString()
    };

    var res = await myFetch(config.apiBase + '/users/' + item.value.item.id + '/password/update', {
        method: 'POST',
        body: JSON.stringify(data)
    }, false);

    if (res.code != 200) {
        showMsg("error", t('pages.users.updatePasswordFailed'));
        return;
    }

    showMsg("success", t('pages.users.updatePasswordSuccess'));
    password1.value = "";
    password2.value = "";
    password3.value = "";
}
</script>

<template>
    <div v-if="loaded">
        <div
            class="header"
            :style="'background-image: url(\'' + bannerSrc + '\');'"
        >
            <div class="header-main d-flex align-normal">
                <img :src="headerSrc" class="avatar" />
                <div class="ml-6">
                    <div class="d-flex align-end">
                        <p class="username">{{ item.item.title }}</p>
                        <p class="userid">(UID:{{ item.item.id }})</p>
                    </div>
                    <p class="userinfo">{{ t('pages.users.info', { submissions: item.item.submissions.total, problems: item.item.problems.accepted.length }) }}</p>
                    <p class="userinfo">{{ t('pages.users.rating', { rating: item.item.rating }) }}</p>
                </div>
            </div>
        </div>
        <div class="button-list">
            <button :class="'button' + (currPage == 'info' ? ' button-clicked' : '')" @click="currPage = 'info'">{{ t('pages.users.infoButton') }}</button>
            <button :class="'button' + (currPage == 'problem' ? ' button-clicked' : '')" @click="currPage = 'problem'">{{ t('pages.users.problemsButton') }}</button>
            <button :class="'button' + (currPage == 'edit' ? ' button-clicked' : '')" @click="currPage = 'edit'" v-if="item.allowEdit">{{ t('pages.users.settingButton') }}</button>
        </div>
        <div class="markdown-text UserDetails-card" v-html="md.render(item.item.info)" v-if="currPage == 'info'"></div>
        <div class="markdown-text UserDetails-card" v-if="currPage == 'problem'" style="padding-bottom: 15px!important;">
            <h2>{{ t('pages.users.triedProblem') }}</h2>
            <li v-for="i in item.item.problems.tried">
                <router-link :to="'/problems/' + i.id">{{ i.alias }} - {{ i.title }}</router-link>
            </li>
            <h2>{{ t('pages.users.acceptedProblem') }}</h2>
            <li v-for="i in item.item.problems.accepted">
                <router-link :to="'/problems/' + i.id">{{ i.alias }} - {{ i.title }}</router-link>
            </li>
        </div>
        <div class="UserDetails-card" v-if="currPage == 'edit' && item.allowEdit">
            <p>
                {{ t('pages.users.uploadHeader') }}：
                <v-btn 
                    size="small"
                    class="UserDetails-button"
                    @click="uploadHeader()"
                    :disabled="!enableUploadBtn1"
                >{{ t('pages.users.uploadHeaderButton') }}</v-btn>
            </p>
            <p class="mt-3">
                {{ t('pages.users.uploadBanner') }}：
                <v-btn 
                    size="small"
                    class="UserDetails-button"
                    @click="uploadBanner()"
                    :disabled="!enableUploadBtn2"
                >{{ t('pages.users.uploadBannerButton') }}</v-btn>
            </p>
            <v-divider class="mt-3"></v-divider>
            <div class="d-flex align-center mt-3">
                <p>{{ t('pages.users.username') }}：</p>
                <v-text-field
                    v-model="username"
                    dense
                    hide-details
                    clearable
                    density="compact"
                ></v-text-field>
            </div>
            <div class="mt-3">
                <p>
                    {{ t('pages.users.introduction') }}：
                    <v-btn
                        size="small"
                        class="UserDetails-button"
                        @click="isPreview = !isPreview"
                    >{{ t('pages.users.preview') }}</v-btn>
                </p>
                <MonacoEditor
                    v-if="!isPreview"
                    language="markdown"
                    theme="vs-dark"
                    height="500"
                    v-model:value="info"
                    style="margin-top: 5px;"
                    :options="{ wordWrap: 'on' }"
                ></MonacoEditor>
                <div
                    v-else
                    class="markdown-text"
                    v-html="md.render(info)"
                    style="min-height: 500px; max-height: 500px; overflow-y: auto;"
                ></div>
            </div>
            <div class="d-flex justify-center mt-3">
                <v-btn 
                    class="submitButton" 
                    @click="submit()"
                    :disabled="!enableBtn"
                >{{ t('pages.users.submit') }}</v-btn>
            </div>
            <v-divider class="mt-3"></v-divider>
            <div class="d-flex align-center mt-3">
                <p>{{ t('pages.users.oldPassword') }}：</p>
                <v-text-field
                    v-model="password1"
                    dense
                    hide-details
                    clearable
                    density="compact"
                    type="password"
                ></v-text-field>
            </div>
            <div class="d-flex align-center mt-3">
                <p>{{ t('pages.users.newPassword') }}：</p>
                <v-text-field
                    v-model="password2"
                    dense
                    hide-details
                    clearable
                    density="compact"
                    type="password"
                ></v-text-field>
            </div>
            <div class="d-flex align-center mt-3">
                <p>{{ t('pages.users.confirmPassword') }}：</p>
                <v-text-field
                    v-model="password3"
                    dense
                    hide-details
                    clearable
                    density="compact"
                    type="password"
                ></v-text-field>
            </div>
            <div class="d-flex justify-center mt-3">
                <v-btn 
                    class="submitButton" 
                    @click="submitPassword()"
                    :disabled="!enableBtn"
                >{{ t('pages.users.submit') }}</v-btn>
            </div>
        </div>
    </div>
</template>

<style lang="css" scoped>
.header {
    height: 250px;
    background-image: url(https://lyoj.littleyang.icu/data/user/1/background.jpg?t=1739776974);
    background-size: cover;
    background-position: center;
}

.header-main {
    padding: 20px;

}

.avatar {
    width: 125px;
    height: 125px;
    border-radius: 50%;
}

.username {
    font-size: 40px;
    font-weight: 200;
    line-height: 50px;
    color: #fff;
    text-shadow: 1px 1px 3px #333;
}

.userid {
    font-size: 30px;
    font-weight: 200;
    color: #fff;
    text-shadow: 1px 1px 3px #333;
}

.userinfo {
    font-size: 20px;
    font-weight: 200;
    color: #fff;
    text-shadow: 1px 1px 3px #333;
}

.button-list {
    padding-left:20px;
    float:bottom;
    height:0px;
}

.button {
    background-color: rgba(0,0,0,0);
    border-color: rgba(0,0,0,0);
    border-radius: 0px;
    height: 40px;
    font-weight: 100;
    font-size: 15px;
    margin-top: 0px;
    position: relative;
    top: -40px;
    color: var(--color-text);
    margin-right: 0px;
    padding: 0px 25px;
    text-shadow: 1px 1px 3px #333;
    transition: color 0.3s, background-color 0.3s, text-shadow 0.3s;
}

.button-clicked {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    text-shadow: revert!important;
}

.UserDetails-card {
    padding: 1rem;
    min-height: 300px;
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    border-bottom-left-radius: 15px;
    border-bottom-right-radius: 15px;
}

.UserDetails-button {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}

.submitButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>