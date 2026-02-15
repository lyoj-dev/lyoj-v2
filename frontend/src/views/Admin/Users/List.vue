<script lang="ts">
import { config, groupColor, maxRating, tagsTypeList } from '@/config';
import { myFetch, showMsg } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref, watchEffect } from 'vue';
import AdminUserSearch from '@/components/Admin/User/Search.vue';
import AdminUserCard from '@/components/Admin/User/Card.vue';
import { goto } from '@/router';
import { i18n } from '@/i18n';
import AdminNavigation from '@/components/Admin/Navigation.vue';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();
    
    var url = config.apiBase + "/admin/users/list";
    if ("page" in to.query) url += "?page=" + to.query["page"];
    else url += "?page=1";
    if ("title" in to.query) url += "&title=" + to.query["title"];
    if ("groups" in to.query) url += "&groups=" + to.query["groups"];
    if ("minRating" in to.query) url += "&minRating=" + to.query["minRating"];
    if ("maxRating" in to.query) url += "&maxRating=" + to.query["maxRating"];
    var json = await myFetch(url);
    var groups = await myFetch(config.apiBase + "/users/listAllGroups");

    next((e: any) => e.loading({
        data: json,
        query: to.query,
        currPage: to.query["page"] ? parseInt(to.query["page"]) : 1,
        groupsList: groups.items
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

const groupsList: any = ref([]);

const title = ref("");
const groups: any = ref([]);
const minRatingValue = ref(0);
const maxRatingValue = ref(maxRating);

const originalQuery: any = ref({});

const selectedList: any = ref([]);
const fullSelected = ref(0);
const selectedOnly = ref(false);

const addGroupDialog = ref(false);
const groupAdd = ref(true);
const addGroupList: any = ref([]);
const addGroupUsersList: any = ref([]);

function loading(data: any) {
    list.value = data.data;
    currPage.value = data.currPage;
    originalQuery.value = data.query;

    for (var item of list.value.items) item.selected = selectedList.value.map((e: any) => e.id).indexOf(item.id) != -1;

    groupsList.value = data.groupsList;

    if ("title" in data.query) title.value = data.query["title"];
    if ("groups" in data.query) groups.value = JSON.parse(data.query["groups"]);
    if ("minRating" in data.query) minRatingValue.value = JSON.parse(data.query["minRating"]);
    if ("maxRating" in data.query) maxRatingValue.value = JSON.parse(data.query["maxRating"]);

    loaded.value = true;
}

defineExpose({ loading });

async function updatePage(data: any) {
    NProgress.start();
    NProgress.inc();
    
    history.pushState(null, "", "/admin/users/list?page=" + (data["page"] || 1) + 
        (data["title"] ? "&title=" + data["title"] : "") + 
        (data["groups"] ? "&groups=" + data["groups"] : "") + 
        (data["minRating"] ? "&minRating=" + data["minRating"] : "") + 
        (data["maxRating"] ? "&maxRating=" + data["maxRating"] : "")
    );
    var url = config.apiBase + "/admin/users/list";
    if ("page" in data) url += "?page=" + data["page"];
    else url += "?page=1";
    if ("title" in data) url += "&title=" + data["title"];
    if ("groups" in data) url += "&groups=" + data["groups"];
    if ("minRating" in data) url += "&minRating=" + data["minRating"];
    if ("maxRating" in data) url += "&maxRating=" + data["maxRating"];
    var json = await myFetch(url);
    var groups = await myFetch(config.apiBase + "/users/listAllGroups");

    loading({
        data: json,
        query: data,
        currPage: data["page"] ? parseInt(data["page"]) : 1,
        groupsList: groups.items
    });

    NProgress.done();
}

async function search() {
    var data: any = {};
    data.page = 1;
    if (title.value != "") data.title = title.value;
    if (groups.value.length) data.groups = JSON.stringify(groups.value);
    if (minRatingValue.value != 0) data.minRating = minRatingValue.value;
    if (maxRatingValue.value != maxRating) data.maxDiff = maxRatingValue.value;

    updatePage(data);
}

function jump() {
    var data: any = {};
    for (var key in originalQuery.value) data[key] = originalQuery.value[key];
    data.page = currPage.value;
    if (selectedOnly.value) {
        loading({
            data: { 
                items: selectedList.value.slice((data.page - 1) * 10, data.page * 10), 
                pageCount: Math.ceil(selectedList.value.length / 10) 
            },
            query: data,
            currPage: data["page"] ? parseInt(data["page"]) : 1,
            groupsList: groupsList.value
        });
    } else {
        updatePage(data);
    }
}

function updateSelectedOnly() {
    selectedOnly.value = !selectedOnly.value;
    currPage.value = 1;
    jump();
}

function addUsersGroup() {
    addGroupUsersList.value = selectedList.value.map((e: any) => e.id);
    groupAdd.value = true;
    addGroupList.value = [];
    addGroupDialog.value = true;
}

function addUserGroup(id: number) {
    addGroupUsersList.value = [ id ];
    groupAdd.value = true;
    addGroupList.value = [];
    addGroupDialog.value = true;
}

async function addGroupSubmit() {
    if (addGroupList.value.length == 0) {
        showMsg('error', t('pages.admin.users.list.noGroup'));
        addGroupDialog.value = false;
        return;
    }
    if (!confirm(t('pages.admin.users.list.addGroupConfirm', { count: addGroupList.value.length }))) return;
    var res = await myFetch(config.apiBase + "/admin/users/addGroup", {
        method: "POST",
        body: JSON.stringify({
            ids: addGroupUsersList.value,
            groups: addGroupList.value
        })
    });
    showMsg('success', t('pages.admin.users.list.addGroupSuccess'));
    for (var item of addGroupUsersList.value) {
        var user = list.value.items.find((e: any) => e.id == item);
        if (user) user.groups = user.groups.concat(groupsList.value.filter((value: any) => addGroupList.value.indexOf(value.id) != -1))
                                           .filter((value: any, index: any, self: any) => self.indexOf(value) === index)
                                           .sort((a: any, b: any) => a.id - b.id);
        user = selectedList.value.find((e: any) => e.id == item);
        if (user) user.groups = user.groups.concat(groupsList.value.filter((value: any) => addGroupList.value.indexOf(value.id) != -1))
                                           .filter((value: any, index: any, self: any) => self.indexOf(value) === index)
                                           .sort((a: any, b: any) => a.id - b.id);
    }
    addGroupDialog.value = false;
}

function removeUsersGroup() {
    addGroupUsersList.value = selectedList.value.map((e: any) => e.id);
    groupAdd.value = false;
    addGroupList.value = [];
    addGroupDialog.value = true;
}

function removeUserGroup(id: number) {
    addGroupUsersList.value = [ id ];
    groupAdd.value = false;
    addGroupList.value = [];
    addGroupDialog.value = true;
}

async function removeGroupSubmit() {
    if (addGroupList.value.length == 0) {
        showMsg('error', t('pages.admin.users.list.noGroup'));
        addGroupDialog.value = false;
        return;
    }
    if (!confirm(t('pages.admin.users.list.removeGroupConfirm', { count: addGroupList.value.length }))) return;
    var res = await myFetch(config.apiBase + "/admin/users/removeGroup", {
        method: "POST",
        body: JSON.stringify({
            ids: addGroupUsersList.value,
            groups: addGroupList.value
        })
    });
    showMsg('success', t('pages.admin.users.list.removeGroupSuccess'));
    for (var item of addGroupUsersList.value) {
        var user = list.value.items.find((e: any) => e.id == item);
        if (user) user.groups = user.groups.filter((value: any) => addGroupList.value.indexOf(value.id) == -1)
                                           .sort((a: any, b: any) => a.id - b.id);
        user = selectedList.value.find((e: any) => e.id == item);
        if (user) user.groups = user.groups.filter((value: any) => addGroupList.value.indexOf(value.id) == -1)
                                           .sort((a: any, b: any) => a.id - b.id);
    }
    addGroupDialog.value = false;
}

function addGroup(id: number) {
    if (groups.value.indexOf(id) == -1) groups.value = groups.value.concat([ id ]);
}

function selected(id: number, value: boolean) {
    var item = list.value.items.find((item: any) => item.id == id);
    item.selected = value;
    if (value) {
        if (selectedList.value.map((e: any) => e.id).indexOf(id) == -1) selectedList.value.push(item);
    } else {
        var index = selectedList.value.map((e: any) => e.id).indexOf(id);
        if (index != -1) selectedList.value.splice(index, 1);
    }
    selectedList.value.sort((a: any, b: any) => a.id - b.id);
}

function fullSelectedClick() {
    var allSelected = fullSelected.value == list.value.items.length;
    for (var item of list.value.items) selected(item.id, !allSelected);
}

watchEffect(() => {
    if (list.value.items == undefined) return;
    fullSelected.value = 0;
    for (var item of list.value.items) if (item.selected) fullSelected.value++;
});
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="users"></AdminNavigation>
        <AdminUserSearch
            v-model:title="title"
            v-model:groups="groups"
            v-model:minRating="minRatingValue"
            v-model:maxRating="maxRatingValue"
            :groupsList="groupsList"
            :disableSearch="selectedOnly"
            @search="search()"
        ></AdminUserSearch>
        <v-card class="d-flex UserCard card-radius">
            <router-link 
                to="/admin/users/create" 
                class="UserCard-status"
                style="color: rgb(36, 140, 36)!important;"
            ><v-icon icon="mdi-plus"></v-icon></router-link>
            <router-link to="/admin/users/create" class="UserCard-problem">{{ t('pages.admin.users.list.addUser') }}</router-link>
        </v-card>
        <v-card class="d-flex UserCard card-radius">
            <div
                class="UserCard-status"
                @click="fullSelectedClick()"
            >
                <v-icon 
                    :icon="fullSelected == 0 ? 'mdi-checkbox-blank-outline' : (fullSelected == list.items.length ? 'mdi-checkbox-marked' : 'mdi-minus-box')"
                ></v-icon>
            </div>
            <div 
                style="width: calc(87% - 37px); padding: 3px;"
            >{{ t('pages.admin.users.list.selectedUser', { count: selectedList.length, selected: fullSelected }) }}</div>
            <div class="UserCard-actions d-flex justify-center align-center">
                <v-btn 
                    class="UserCard-actionButton" 
                    icon="mdi-plus" 
                    size="x-small"
                    :title="t('pages.admin.users.list.addSelectedButton')"
                    :disabled="selectedList.length == 0"
                    @click="addUsersGroup()"
                ></v-btn>
                <v-btn 
                    class="UserCard-actionButton" 
                    icon="mdi-minus" 
                    size="x-small"
                    :title="t('pages.admin.users.list.removeSelectedButton')"
                    :disabled="selectedList.length == 0"
                    @click="removeUsersGroup()"
                ></v-btn>
                <v-btn 
                    class="UserCard-actionButton" 
                    :icon="selectedOnly ? 'mdi-eye-off' : 'mdi-eye'" 
                    size="x-small"
                    :title="t('pages.admin.users.list.' + (selectedOnly ? 'showAllButton' : 'showSelectedButton'))"
                    :disabled="selectedList.length == 0"
                    @click="updateSelectedOnly()"
                ></v-btn>
            </div>
        </v-card>
        <AdminUserCard
            v-for="item in list.items"
            :selected="item.selected"
            :id="item.id"
            :title="item.title"
            :groups="item.groups"
            :rating="item.rating"
            :createTime="item.createTime"
            @addGroup="addGroup(item.id)"
            @addUserGroup="addUserGroup(item.id)"
            @removeUserGroup="removeUserGroup(item.id)"
            @updateSelected="(value) => selected(item.id, value)"
        ></AdminUserCard>
        <v-pagination
            v-model="currPage"
            :length="list.pageCount"
            :total-visible="8"
            @update:model-value="jump()"
        ></v-pagination>
        <v-dialog
            v-model="addGroupDialog"
            width="400px"
        >
            <v-card class="card-radius AddGroupDialog">
                <v-card-title>{{ t(groupAdd == true ? 'pages.admin.users.list.addGroup' : 'pages.admin.users.list.removeGroup') }}</v-card-title>
                <v-card-text>
                    <div class="d-flex align-center mb-3">
                        <p style="flex-shrink: 0;">{{ t('pages.admin.users.list.groups') }}：</p>
                        <v-autocomplete
                            v-model="addGroupList"
                            :items="groupsList"
                            density="compact"
                            item-title="title"
                            item-value="id"
                            multiple
                            hide-details
                            clearable
                            chips
                        >
                            <template v-slot:chip="{ item, }">
                                <v-chip
                                    size="small" 
                                    :color="groupColor" 
                                    variant="flat"
                                >{{ (item.raw as any).title }}</v-chip>
                            </template>
                        </v-autocomplete>
                    </div>
                    <div class="d-flex justify-center mt-3">
                        <v-btn 
                            class="submitButton"
                            @click="groupAdd == true ? addGroupSubmit() : removeGroupSubmit()" 
                        >{{ t('pages.admin.users.list.submit') }}</v-btn>
                    </div>
                </v-card-text>
            </v-card>
        </v-dialog>
    </div>
</template>

<style lang="css" scoped>
.UserCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 0px 30px;
    width: 100%;
    min-height: 50px;
    margin-bottom: 20px;
    align-items: center;
}

.UserCard-status {
    display: block;
    padding: 3px;
    width: 3%;
    position: relative;
    top: -0.8px;
    cursor: pointer;
}

.UserCard-actions {
    width: calc(10% + 37px);
    gap: 5px;
}

.UserCard-actionButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}

.AddGroupDialog {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px;
}
</style>