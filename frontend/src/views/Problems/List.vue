<script lang="ts">
import { config } from '@/config';
import { goto } from '@/router';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import ProblemCard from '@/components/Problem/Card.vue';
import ProblemSearch from '@/components/Problem/Search.vue';
import { appBarConfig } from '@/components/AppBar/config';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    var url = config.apiBase + "/problems/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    if ("title" in to.query) url += "&title=" + to.query["title"];
    if ("tags" in to.query) url += "&tags=" + to.query["tags"];
    if ("difficulties" in to.query) url += "&difficulties=" + to.query["difficulties"];
    var response = await fetch(url);
    if (response.status != 200) goto("error", { code: response.status });
    var json = await response.json();

    var response = await fetch(config.apiBase + "/problems/listAllTags");
    if (response.status != 200) goto("error", { code: response.status });
    var tags = await response.json();

    next((e: any) => e.loading({
        data: json,
        query: to.query,
        currPage: to.query["page"] ? parseInt(to.query["page"]) : 1,
        tagsList: tags.items
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script setup lang="ts">
const list: any = ref([]);
const loaded = ref(false);
const currPage = ref(1);

const tagsList: any = ref([]);

const title = ref("");
const tags = ref([]);
const difficulties = ref([]);

const originalQuery: any = ref({});

function loading(data: any) {
    list.value = data.data;
    currPage.value = data.currPage;
    originalQuery.value = data.query;

    tagsList.value = data.tagsList;

    if ("title" in data.query) title.value = data.query["title"];
    if ("tags" in data.query) tags.value = JSON.parse(data.query["tags"]);
    if ("difficulties" in data.query) difficulties.value = JSON.parse(data.query["difficulties"]);

    loaded.value = true;
}

defineExpose({ loading });

function search() {
    var data: any = {};
    data.page = 1;
    if (title.value != "") data.title = title.value;
    if (tags.value.length) data.tags = JSON.stringify(tags.value);
    if (difficulties.value.length) data.difficulties = JSON.stringify(difficulties.value);
    goto('problemsList', data);
}

function jump() {
    var data: any = {};
    for (var key in originalQuery.value) data[key] = originalQuery.value[key];
    data.page = currPage.value;
    goto('problemsList', data);
}
</script>

<template>
    <div v-if="loaded">
        <ProblemSearch
            v-model:tags="tags"
            v-model:difficulties="difficulties"
            v-model:title="title"
            :tagsList="tagsList"
            @search="search"
        ></ProblemSearch>
        <ProblemCard
            v-for="item in list.items"
            :key="item.id"
            :status="item.status"
            :id="item.id"
            :alias="item.alias"
            :title="item.title"
            :tags="item.tags"
            :accepted="item.accepted"
            :total="item.total"
            :difficulty="item.difficulty"
        ></ProblemCard>
        <v-pagination
            v-model="currPage"
            :length="list.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
    </div>
</template>