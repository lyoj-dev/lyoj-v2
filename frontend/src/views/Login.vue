<script lang="ts">
import { config } from '@/config';
import { i18n } from '@/i18n';
import { ref, defineComponent } from 'vue';
import crypto from 'crypto-js';
import { useRouter } from 'vue-router';
import { myFetch, setCookie, showMsg, sleep } from '@/utils';
import NProgress from 'nprogress';
import { goto } from '@/router';
export default defineComponent({
    async beforeRouteEnter(to, from, next) {
        NProgress.start();
        NProgress.inc();
        var data = await myFetch(config.apiBase + "/users/check");
        next((e: any) => e.loading({
            data: data
        }));
    },
});
</script>

<script setup lang="ts">
const t = i18n.global.t;
const router = useRouter();
const enableBtn = ref(true);

const user = ref(''); // 支持用户名/学号登录
const passwd = ref(''); // 正常密码

async function login() {
    enableBtn.value = false;
    var data = {
        user: user.value,
        passwd: crypto.MD5(passwd.value).toString()
    };
    var res = await myFetch(config.apiBase + "/users/login", {
        method: 'POST',
        body: JSON.stringify(data)
    });
    if (res.success) {
        setCookie("session", res.session, 30 * 24 * 60 * 60);
        showMsg("success", t('pages.login.loginSuccess'));
        await sleep(1000);
        router.go(-1);
    } else {
        showMsg("error", t(res["failMsg"]));
        enableBtn.value = true;
        await sleep(1000);
    }
}

async function loading(data: any) {
    if (data.data.loginAs != 0) {
        router.go(-1);
    }
}

defineExpose({ loading });
</script>

<template>
    <v-card class="Login card-radius">
        <v-card-title>{{ t('pages.login.title') }}</v-card-title>
        <v-card-text>
            <div class="d-flex align-center">
                <p>{{ t('pages.login.user') }}：</p>
                <v-text-field
                    v-model="user"
                    :placeholder="t('pages.login.userPlaceholder')"
                    density="compact"
                    hide-details
                ></v-text-field>
            </div>
            <div class="d-flex align-center mt-4">
                <p>{{ t('pages.login.password') }}：</p>
                <v-text-field
                    v-model="passwd"
                    :placeholder="t('pages.login.passwordPlaceholder')"
                    type="password"
                    density="compact"
                    hide-details
                ></v-text-field>
            </div>
            <div class="d-flex justify-center">
                <v-btn
                    class="mt-4 Login-button"
                    @click="login"
                    :disabled="!enableBtn"
                >{{ t('pages.login.loginButton') }}</v-btn>    
            </div>
            <v-divider class="mt-4"></v-divider>
            <div class="d-flex justify-center mt-4 align-center">
                {{ t('pages.login.casLogin') }}：
                <v-btn
                    class="Login-button d-flex align-center"
                    @click="goto('casLogin', {})"
                >
                    <img src="/favicon.ico" style="width: 20px; height: 20px;"/>
                    <p style="line-height: 15px; position: relative; top: -1px;">&nbsp;{{ t('pages.login.casLoginButton') }}</p>
                </v-btn>
            </div>
        </v-card-text>
    </v-card>
</template>

<style scoped>
.Login {
    padding: 5px; 
    margin: auto; 
    background-color: var(--color-background);
    color: var(--color-text);
    width: 600px;
}

.Login-button {
    background-color: var(--color-background-soft);
    color: var(--color-text);
}
</style>