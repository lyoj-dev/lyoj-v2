<script lang="ts">
import { config, tagsTypeList } from '@/config';
import { myFetch, showMsg } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import AdminContestCard from '@/components/Admin/Contest/Card.vue';
import { goto } from '@/router';
import { i18n } from '@/i18n';
import AdminNavigation from '@/components/Admin/Navigation.vue';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    
    var url = config.apiBase + "/problems/listAllTags";
    var json = await myFetch(url);

    next((e: any) => e.loading({
        data: json.items,
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



function loading(data: any) {

    loaded.value = true;
}

defineExpose({ loading });
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="groups"></AdminNavigation>

    </div>
</template>

<style lang="css" scoped>
</style>