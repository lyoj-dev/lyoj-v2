<script lang="ts">
import { config } from '@/config';
import { myFetch } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import AdminContestCard from '@/components/Admin/Contest/Card.vue';
import { goto } from '@/router';
import { i18n } from '@/i18n';
import AdminNavigation from '@/components/Admin/Navigation.vue';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    
    var url = config.apiBase + "/contests/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    var json = await myFetch(url);

    next((e: any) => e.loading({
        data: json,
        query: to.query,
        currPage: to.query["page"] ? parseInt(to.query["page"]) : 1,
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

const items: any = ref([]);
const currPage = ref(1);
const originalQuery: any = ref({});

function loading(data: any) {
    items.value = data.data;
    currPage.value = data.currPage;
    originalQuery.value = data.query;
    loaded.value = true;
}

defineExpose({ loading });

function jump() {
    var data: any = {};
    for (var key in originalQuery.value) data[key] = originalQuery.value[key];
    data.page = currPage.value;
    goto('adminContestsList', data);
}

function edit(id: number) {
    goto('contestEdit', {}, { id: id });
}
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="contests"></AdminNavigation>
        <v-card class="d-flex ContestCard card-radius" v-if="items.allowCreate">
            <router-link 
                to="/contests/0/edit" 
                class="ContestCard-status"
                style="color: rgb(36, 140, 36)!important;"
            ><v-icon icon="mdi-plus"></v-icon></router-link>
            <router-link to="/contests/0/edit" class="ContestCard-problem">{{ t('pages.contests.addContest') }}</router-link>
        </v-card>
        <AdminContestCard
            v-for="item in items.items"
            :id="item.id"
            :title="item.title"
            :tags="item.tags"
            :startTime="item.starttime"
            :endTime="item.duration + item.starttime"
            :problems="item.problems"
            :status="item.status"
            @edit="edit(item.id)"
        ></AdminContestCard>
        <v-pagination
            v-model="currPage"
            :length="items.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
    </div>
</template>

<style lang="css" scoped>
.ContestCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.ContestCard-status {
    width: 3%;
    position: relative;
    top: -0.8px;
}

.ContestCard-problem {
    width: 97%;
}
</style>