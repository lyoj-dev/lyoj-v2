<script lang="ts">
import { config, maxDifficulty } from '@/config';
import { goto } from '@/router';
import NProgress from 'nprogress';
import { defineComponent, getCurrentInstance, ref } from 'vue';
import ProblemCard from '@/components/Problem/Card.vue';
import ProblemSearch from '@/components/Problem/Search.vue';
import { myFetch, showMsg, sleep } from '@/utils';
import { i18n } from '@/i18n';
import { useRouter } from 'vue-router';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    
    var url = config.apiBase + "/problems/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    if ("title" in to.query) url += "&title=" + to.query["title"];
    if ("tags" in to.query) url += "&tags=" + to.query["tags"];
    if ("minDiff" in to.query) url += "&minDiff=" + to.query["minDiff"];
    if ("maxDiff" in to.query) url += "&maxDiff=" + to.query["maxDiff"];
    var json = await myFetch(url);
    var tags = await myFetch(config.apiBase + "/problems/listAllTags");

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
const t = i18n.global.t;
const router = useRouter();

const list: any = ref([]);
const loaded = ref(false);
const currPage = ref(1);

const tagsList: any = ref([]);

const title = ref("");
const tags: any = ref([]);
const minDiff = ref(0);
const maxDiff = ref(maxDifficulty);

const originalQuery: any = ref({});

function loading(data: any) {
    list.value = data.data;
    currPage.value = data.currPage;
    originalQuery.value = data.query;

    tagsList.value = data.tagsList;

    if ("title" in data.query) title.value = data.query["title"];
    if ("tags" in data.query) tags.value = JSON.parse(data.query["tags"]);
    if ("minDiff" in data.query) minDiff.value = JSON.parse(data.query["minDiff"]);
    if ("maxDiff" in data.query) maxDiff.value = JSON.parse(data.query["maxDiff"]);

    loaded.value = true;
}

defineExpose({ loading });

function search() {
    var data: any = {};
    data.page = 1;
    if (title.value != "") data.title = title.value;
    if (tags.value.length) data.tags = JSON.stringify(tags.value);
    if (minDiff.value != 0) data.minDiff = minDiff.value;
    if (maxDiff.value != maxDifficulty) data.maxDiff = maxDiff.value;
    goto('problemsList', data);
}

function jump() {
    var data: any = {};
    for (var key in originalQuery.value) data[key] = originalQuery.value[key];
    data.page = currPage.value;
    goto('problemsList', data);
}

function addTag(id: number) {
    if (tags.value.indexOf(id) == -1) tags.value = tags.value.concat([ id ]);
}

async function deleteProblem(id: number, name: string) {
    if (!confirm(t('pages.problems.deleteConfirm', { name: name }))) return;
    var res = await myFetch(config.apiBase + "/problems/" + id + "/delete", { method: "POST" });
    showMsg("success", t('pages.problems.deleteSuccess'));
    await sleep(1000);
    load(
        router.currentRoute.value, 
        router.currentRoute.value, 
        (func: any) => func({ loading: (data: any) => {
            loading(data);
            NProgress.done();
        }
    }));
}
</script>

<template>
    <div v-if="loaded">
        <ProblemSearch
            v-model:tags="tags"
            v-model:minDiff="minDiff"
            v-model:maxDiff="maxDiff"
            v-model:title="title"
            :tagsList="tagsList"
            @search="search"
        ></ProblemSearch>
        <v-card class="d-flex ProblemCard card-radius" v-if="list.allowCreate">
            <router-link 
                to="/problems/0/edit" 
                class="ProblemCard-status"
                style="color: rgb(36, 140, 36)!important;"
            ><v-icon icon="mdi-plus"></v-icon></router-link>
            <router-link to="/problems/0/edit" class="ProblemCard-problem">{{ t('pages.problems.addProblem') }}</router-link>
        </v-card>
        <ProblemCard
            v-for="item in list.items"
            :status="item.status"
            :id="item.id"
            :alias="item.alias"
            :title="item.title"
            :tags="item.tags"
            :accepted="item.accepted"
            :total="item.total"
            :difficulty="item.difficulty"
            :allowEdit="item.allowEdit"
            :allowDelete="item.allowDelete"
            @addTag="addTag"
            @deleteProblem="deleteProblem"
        ></ProblemCard>
        <v-pagination
            v-model="currPage"
            :length="list.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
    </div>
</template>

<style lang="css" scoped>
.ProblemCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.ProblemCard-status {
    width: 3%;
    position: relative;
    top: -0.8px;
}

.ProblemCard-problem {
    width: 97%;
}
</style>