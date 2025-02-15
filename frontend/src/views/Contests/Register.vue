<script lang="ts">
import { config } from '@/config';
import { myFetch, showMsg, sleep } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref } from 'vue';
import { useRoute } from 'vue-router';
import ContestNavigation from '@/components/Contest/Navigation.vue';
import ContestTitle from '@/components/Contest/Title.vue';
import { i18n } from '@/i18n';
import { goto, locate } from '@/router';
import crypto from 'crypto-js';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var json = await myFetch(config.apiBase + '/contests/' + to.params.id);
    
    next((e: any) => e.loading({
        data: json
    }));
}
export default defineComponent({
    async beforeRouteEnter(to, from, next) { await load(to, from, next); },
    async beforeRouteUpdate(to, from) { await load(to, from, (func: any) => func(this)); },
});
</script>

<script lang="ts" setup>
const loaded = ref(false);
const data: any = ref({});
const route = useRoute();
const password = ref("");
const enableBtn = ref(true);
const t = i18n.global.t;

function loading(data2: any) {
    data.value = data2.data;
    loaded.value = true;
}

defineExpose({ loading });

async function register() {
    enableBtn.value = false;
    var res = await myFetch(config.apiBase + '/contests/' + route.params.id + '/register', {
        method: 'POST',
        body: JSON.stringify({
            password: password.value == '' ? '' : crypto.MD5(password.value).toString()
        })
    }, false);
    if (res.code == 200) {
        showMsg("success", t('pages.contests.registerSuccess'));
        await sleep(1000);
        locate('/contests/' + route.params.id + '/index');
        return;
    }
    showMsg("error", t('pages.contests.registerFailed'));
    await sleep(1000);
    enableBtn.value = true;
}
</script>

<template>
    <div v-if="loaded">
        <ContestNavigation 
            current="register" 
            :id="route.params.id" 
            :identity="data.identity" 
            :signup="data.signup"
            :endTime="data.item.starttime + data.item.duration"
        ></ContestNavigation>
        <ContestTitle
            :id="data.item.id"
            :title="data.item.title"
            :startTime="data.item.starttime"
            :endTime="data.item.starttime + data.item.duration"
            :problems="data.item.problemNumber"
            :signups="data.item.signups"
        ></ContestTitle>
        <v-card class="ContestVCard card-radius">
            <v-card-title>{{ t('pages.contests.register') }}</v-card-title>
            <v-card-text>
                <div class="d-flex align-center" v-if="data.item.hasPassword">
                    <p>{{ t('pages.contests.password') }}：</p>
                    <v-text-field 
                        v-model="password" 
                        type="password"
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex justify-center mt-3">
                    <v-btn 
                        class="submitButton" 
                        @click="register()"
                        :disabled="!enableBtn"
                    >{{ t('pages.contests.registerButton') }}</v-btn>
                </div>
            </v-card-text>
        </v-card>
    </div>
</template>

<style scoped>
.ContestVCard {
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