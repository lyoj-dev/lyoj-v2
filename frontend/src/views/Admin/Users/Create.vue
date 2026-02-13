<script lang="ts">
import { config, groupColor, idTypes, sexTypes, unitTypes } from '@/config';
import { myFetch, showMsg, sleep } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import AdminNavigation from '@/components/Admin/Navigation.vue';
import { i18n } from '@/i18n';
import { locate } from '@/router';
import crypto from 'crypto-js';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var groups = await myFetch(config.apiBase + "/users/listAllGroups");

    next((e: any) => e.loading({
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
const title = ref("");
const name = ref("");
const idNumber = ref("");
const idType = ref(idTypes[0].id);
const unitId = ref(unitTypes[0].id);
const sex = ref(sexTypes[0].id);
const password = ref("");
const groups = ref([]);

const groupsList = ref([]);
const t = i18n.global.t;

function loading(data: any) {
    groupsList.value = data.groupsList;

    loaded.value = true;
}

defineExpose({ loading });

async function submit() {
    if (title.value.trim() == "") {
        showMsg('error', t('pages.admin.users.create.titleIsEmpty'));
        return;
    }

    var data = {
        title: title.value,
        name: name.value,
        idNumber: idNumber.value,
        idType: idType.value,
        typeName: idTypes.find((x) => x.id == idType.value)?.title || "",
        unitId: unitId.value,
        unitName: unitTypes.find((x) => x.id == unitId.value)?.title || "",
        sex: sex.value,
        password: password.value == "" ? "" : crypto.MD5(password.value).toString(),
        groups: groups.value,
    }

    var res = await myFetch(config.apiBase + "/admin/users/create", {
        method: "POST",
        body: JSON.stringify(data),
    });

    showMsg("success", t('pages.admin.users.create.createSuccess'));
    await sleep(1000);
    locate("/users/" + res.id);
}
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="users"></AdminNavigation>
        <v-card class="UserCreate-card card-radius">
            <v-card-title>{{ t('pages.admin.users.create.createTitle') }}</v-card-title>
            <v-card-text>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.create.title') }}：</p>
                    <v-text-field
                        v-model="title"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.create.name') }}：</p>
                    <v-text-field
                        v-model="name"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.create.idNumber') }}：</p>
                    <v-text-field
                        v-model="idNumber"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.create.idType') }}：</p>
                    <v-select
                        v-model="idType"
                        :items="idTypes"
                        item-text="title"
                        item-value="id"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-select>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.create.unitId') }}：</p>
                    <v-select
                        v-model="unitId"
                        :items="unitTypes"
                        item-text="title"
                        item-value="id"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-select>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.create.sex') }}：</p>
                    <v-select
                        v-model="sex"
                        :items="sexTypes"
                        item-text="title"
                        item-value="id"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-select>
                </div>
                <div class="d-flex align-center mb-3">
                    <p>{{ t('pages.admin.users.create.password') }}：</p>
                    <v-text-field
                        v-model="password"
                        type="password"
                        dense
                        hide-details
                        clearable
                        density="compact"
                        :placeholder="t('pages.admin.users.create.passwordHint')"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mb-3">
                    <p style="flex-shrink: 0;">{{ t('pages.admin.users.create.groups') }}：</p>
                    <v-autocomplete
                        v-model="groups"
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
                        @click="submit()"
                    >{{ t('pages.admin.users.create.submit') }}</v-btn>
                </div>
            </v-card-text>
        </v-card>
    </div>
</template>

<style scoped>
.UserCreate-card {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    width: 100%;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px;
}

.submitButton {
    color: var(--color-text)!important;
    background-color: var(--color-background-mute)!important;
}
</style>