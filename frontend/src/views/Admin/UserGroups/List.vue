<script lang="ts">
import { config, tagsTypeList } from '@/config';
import { myFetch, showMsg } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import AdminUserGroupCard from '@/components/Admin/UserGroup/Card.vue';
import { goto, locate } from '@/router';
import { i18n } from '@/i18n';
import AdminNavigation from '@/components/Admin/Navigation.vue';
import { editor } from 'monaco-editor';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    
    var url = config.apiBase + "/admin/userGroups/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    else url += "?page=1";
    var json = await myFetch(url);
    var users = await myFetch(config.apiBase + "/users/listAll");

    for (var i = 0; i < json.items.length; i++) 
        for (var j = 0; j < json.items[i].users.length; j++) 
            json.items[i].users[j] = json.items[i].users[j].uid;

    next((e: any) => e.loading({
        data: json,
        currPage: "page" in to.query ? parseInt(to.query["page"]) : 1,
        usersList: users,
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

const list: any = ref({});
const currPage = ref(1);
const usersList: any = ref([]);

function loading(data: any) {
    list.value = data.data;
    currPage.value = data.currPage;
    usersList.value = data.usersList.items;
    usersList.value.push({ id: 0, title: "Anonymous" });

    loaded.value = true;
}

defineExpose({ loading });

function jump() {
    locate("/admin/usergroups/list?page=" + currPage.value);
}

async function submit(id: number) {

}
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="groups"></AdminNavigation>
        <AdminUserGroupCard
            v-for="item in list.items"
            :key="item.id"
            :id="item.id"
            :count="item.userCount"
            :usersList="usersList"
            v-model:title="item.title"
            v-model:description="item.description"
            v-model:permission="item.permission"
            v-model:users="item.users"
            @submit="submit(item.id)"
        ></AdminUserGroupCard>
        <v-pagination
            v-model="currPage"
            :length="list.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
    </div>
</template>

<style lang="css" scoped>
</style>