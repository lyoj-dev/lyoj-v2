<script lang="ts">
import { config } from '@/config';
import { i18n } from '@/i18n';
import { defineComponent } from 'vue';
import { useRouter } from 'vue-router';
import { deleteCookie, myFetch, showMsg } from '@/utils';
import NProgress from 'nprogress';
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

async function loading(data: any) {
    if (data.data.loginAs == 0) {
        router.go(-1);
    } else {
        deleteCookie("session");
        showMsg("success", t('pages.logout.success'));
        router.go(-1);
    }
}

defineExpose({ loading });
</script>

<template>
</template>