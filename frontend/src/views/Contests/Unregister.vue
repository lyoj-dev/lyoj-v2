<script lang="ts">
import { config } from '@/config';
import { i18n } from '@/i18n';
import { defineComponent } from 'vue';
import { useRoute } from 'vue-router';
import { deleteCookie, myFetch, showMsg } from '@/utils';
import NProgress from 'nprogress';
import { locate } from '@/router';
export default defineComponent({
    async beforeRouteEnter(to, from, next) {
        NProgress.start();
        NProgress.inc();
        var data = await myFetch(config.apiBase + "/contests/" + to.params.id + "/unregister");
        next((e: any) => e.loading({
            data: data
        }));
    },
});
</script>

<script setup lang="ts">
const t = i18n.global.t;
const route = useRoute();

async function loading(data: any) {
    showMsg("success", t('pages.contests.unregisterSuccess'));
    locate('/contests/' + route.params.id + '/index');
}

defineExpose({ loading });
</script>

<template>
</template>