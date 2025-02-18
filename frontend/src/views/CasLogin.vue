<script lang="ts">
import { config } from '@/config';
import { i18n } from '@/i18n';
import { goto, router } from '@/router';
import { myFetch, setCookie, showMsg, sleep } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import { useRouter } from 'vue-router';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var data = await myFetch(config.apiBase + "/users/check");
    var ticket = to.query.ticket;
    var res: any = null;
    if (ticket != undefined) {
        res = await myFetch(config.apiBase + "/users/casLogin?ticket=" + ticket, {});
    }

    next((e: any) => e.loading({
        ticket: ticket,
        data: res,
        userData: data
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const loaded = ref(false);
const t = i18n.global.t;

async function loading(data: any) {
    if (data.userData.loginAs != 0) {
        router.go(-1);
    }

    if (data.ticket == undefined) {
        location.href = config.casBase + '?service=' + encodeURIComponent(location.href);
        return;
    } else {
        var res = data.data;
        if (res.success) {
            setCookie("session", res.session, 30 * 24 * 60 * 60);
            showMsg("success", t('pages.login.loginSuccess'));
            await sleep(1000);
            goto('index', {});
        } else {
            showMsg("error", res["msg"] + ": " + res["failureCode"] + '<br/>' + res["failureMsg"]);
            await sleep(1000);
            goto('login', {});
        }
    }

    loaded.value = true;
}

defineExpose({ loading });
</script>

<template>
    <div v-if="loaded">

    </div>
</template>