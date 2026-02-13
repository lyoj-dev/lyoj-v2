<script lang="ts">
import { config } from '@/config';
import { myFetch } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import AdminNavigation from '@/components/Admin/Navigation.vue';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    next((e: any) => e.loading({

    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const loaded = ref(false);

function loading(data: any) {

    loaded.value = true;
}

defineExpose({ loading });
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="users"></AdminNavigation>
    </div>
</template>