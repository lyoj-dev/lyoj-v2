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

const tags: any = ref([]);

const addTagId = ref(0);
const addTagDialog = ref(false);
const addTagTitle = ref('');
const addTagType = ref(0);

function loading(data: any) {
    tags.value = data.data;
    loaded.value = true;
}

async function addTagSubmit() {
    var title = addTagTitle.value;
    var type = addTagType.value;
    if (title == "") {
        showMsg('error', t('pages.problems.edit.addTagTitleEmpty'));
        addTagDialog.value = false;
        return;
    }
    var res = await myFetch(config.apiBase + "/problems/addTag", {
        method: 'POST',
        body: JSON.stringify({ title: title, type: type })
    }, false);
    if (res.code != 200) {
        showMsg('error', t('pages.problems.edit.addTagFailed'));
        addTagDialog.value = false;
        return;
    }
    showMsg('success', t('pages.problems.edit.addTagSuccess'));
    tags.value.push({ id: res.id, title: title, type: type });
    addTagDialog.value = false; 
}

async function renameTagSubmit() {
    var id = addTagId.value;
    var title = addTagTitle.value;
    var type = addTagType.value;
    if (title == "") {
        showMsg('error', t('pages.admin.tags.list.renameTagTitleEmpty'));
        addTagDialog.value = false;
        return;
    }
    var res = await myFetch(config.apiBase + "/admin/problems/renameTag", {
        method: 'POST',
        body: JSON.stringify({ id: id, title: title, type: type })
    }, false);
    if (res.code != 200) {
        showMsg('error', t('pages.admin.tags.list.renameTagFailed'));
        addTagDialog.value = false;
        return;
    }
    showMsg('success', t('pages.admin.tags.list.renameTagSuccess'));
    var index = tags.value.map((e: any) => e.id).indexOf(id);
    if (index != -1) {
        tags.value[index].title = title;
        tags.value[index].type = type;
    }
    addTagDialog.value = false; 
}

async function deleteTag(id: number) {
    var res = await myFetch(config.apiBase + "/admin/problems/checkTag?id=" + id);
    var count = res["count"];
    if (!confirm(t('pages.admin.tags.list.deleteTagConfirm', { 
        name: tags.value.find((e: any) => e.id === id)?.title, 
        countProblem: count["problem"],
        countContest: count["contest"],
    }))) return;
    res = await myFetch(config.apiBase + "/admin/problems/deleteTag", {
        method: 'POST',
        body: JSON.stringify({ id: id })
    });
    showMsg('success', t('pages.admin.tags.list.deleteTagSuccess'));
    var index = tags.value.map((e: any) => e.id).indexOf(id);
    if (index != -1) tags.value.splice(index, 1);
}

defineExpose({ loading });
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="tags"></AdminNavigation>
        <v-expansion-panels :model-value="[0]" v-for="tagType in tagsTypeList">
            <v-expansion-panel class="TagsTypeList card-radius" static>
                <v-expansion-panel-title class="d-flex align-center" style="padding: 0px 20px 0px 10px;">
                    <p class="v-card-title" style="padding-right: 0px">{{ tagType.title }}</p>
                </v-expansion-panel-title>
                <v-expansion-panel-text style="padding-bottom: 0px;">
                    <div class="TagsList d-flex">
                        <v-chip 
                            :model-value="true"
                            v-for="tag in tags.filter((e: any) => e.type == tagType.id)"
                            :key="tag" 
                            size="small" 
                            :color="tagsTypeList[tag.type].color" 
                            variant="flat"
                            @click="addTagId = tag.id; addTagTitle = tag.title; addTagType = tag.type; addTagDialog = true"
                        >
                            {{ tag.title }}
                            <v-icon 
                                icon="mdi-close" 
                                @click="deleteTag(tag.id); $event.stopPropagation();"
                                style="position: relative; top: 0.5px"
                            ></v-icon>
                        </v-chip>
                        <v-chip
                            :key="'add' + tagType.id"
                            size="small" 
                            :color="tagsTypeList[tagType.id].color" 
                            variant="flat"
                            :title="t('pages.admin.tags.list.addTag', { type: tagType.title })"
                            @click="addTagId = 0; addTagTitle = ''; addTagType = tagType.id; addTagDialog = true"
                        ><v-icon style="position: relative; top: 0.5px">mdi-plus</v-icon></v-chip>
                    </div>
                </v-expansion-panel-text>
            </v-expansion-panel>
        </v-expansion-panels>
        <v-dialog
            v-model="addTagDialog"
            width="400px"
        >
            <v-card class="card-radius AddTagDialog">
                <v-card-title>{{ t(addTagId == 0 ? 'pages.problems.edit.addTag' : 'pages.admin.tags.list.renameTag') }}</v-card-title>
                <v-card-text>
                    <div class="d-flex align-center mb-3">
                        {{ t('pages.problems.edit.addTagTitle') }}：
                        <v-text-field
                            v-model="addTagTitle"
                            dense
                            hide-details
                            clearable
                            density="compact"
                        ></v-text-field>
                    </div>
                    <div class="d-flex align-center mb-3">
                        {{ t('pages.problems.edit.addTagType') }}：
                        <v-select
                            v-model="addTagType"
                            :items="tagsTypeList"
                            item-title="title"
                            item-value="id"
                            dense
                            hide-details
                        ></v-select>
                    </div>
                    <div class="d-flex justify-center">
                        <v-btn 
                            class="submitButton" 
                            @click="addTagId == 0 ? addTagSubmit() : renameTagSubmit()" 
                        >{{ t('pages.problems.edit.submit') }}</v-btn>
                    </div>
                </v-card-text>
            </v-card>
        </v-dialog>
    </div>
</template>

<style lang="css" scoped>
.TagsTypeList {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    padding: 5px;
    margin-bottom: 20px;
}

.TagsList {
    gap: 5px;
    flex-wrap: wrap;
    padding: 10px 0px;
}

.AddTagDialog {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px;
}
</style>