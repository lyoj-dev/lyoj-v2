<script lang="ts" setup>
import { i18n } from '@/i18n';
import { loginAs, loginInfo } from '@/utils';
import { ref } from 'vue';

const props = defineProps([ 'id', 'current', 'identity', 'signup', 'endTime' ]);

const unused = ref([]);
const curr = ref([ props.current ]);
const t = i18n.global.t;
</script>

<template>
    <v-layout>
        <v-navigation-drawer
            expand-on-hover
            rail
            style="margin-top: 53px;"
            class="NavigationDrawer"
        >
            <v-list>
                <v-list-item
                    prepend-icon="mdi-account"
                    :subtitle="t('pages.contests.visitAs') + '：' + (identity ? (identity == 2 ? t('pages.contests.admin') : t('pages.contests.contestant')) : t('pages.contests.visitor'))"
                    :title="loginInfo.title"
                ></v-list-item>
            </v-list>

            <v-divider></v-divider>

            <v-list 
                density="compact" nav v-model:selected="unused" @update:selected="unused = []"
                v-if="endTime >= Math.floor(Date.now() / 1000) && loginAs != 0"
            >
                <v-list-item prepend-icon="mdi-hand-pointing-right" :title="t('pages.contests.register')" value="signup" :to="'/contests/' + id + '/register'" v-if="!signup"></v-list-item>
                <v-list-item prepend-icon="mdi-power" :title="t('pages.contests.unregister')" :to="'/contests/' + id + '/unregister'" value="signup" v-else></v-list-item>
            </v-list>

            <v-divider
                v-if="endTime >= Math.floor(Date.now() / 1000) && loginAs != 0"
            ></v-divider>

            <v-list density="compact" nav v-model:selected="curr">
                <v-list-item prepend-icon="mdi-home" :title="t('pages.contests.index')" value="index" :to="'/contests/' + id + '/index'"></v-list-item>
                <v-list-item prepend-icon="mdi-format-list-bulleted" :title="t('pages.contests.problems')" value="problems" :to="'/contests/' + id + '/problems/list'"></v-list-item>
                <v-list-item prepend-icon="mdi-server" :title="t('pages.contests.submissions')" value="submissions" :to="'/contests/' + id + '/submissions/list'"></v-list-item>
                <v-list-item prepend-icon="mdi-trophy-award" :title="t('pages.contests.ranking')" value="ranking" :to="'/contests/' + id + '/ranking'"></v-list-item>
                <v-list-item prepend-icon="mdi-pen" :title="t('pages.contests.editButton')" value="edit" :to="'/contests/' + id + '/edit'" v-if="identity == 2"></v-list-item>
            </v-list>
        </v-navigation-drawer>    
    </v-layout>
</template>

<style scoped>
.NavigationDrawer {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
}
</style>