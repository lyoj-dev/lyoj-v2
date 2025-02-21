<script lang="ts">
import { config } from '@/config';
import { myFetch } from '@/utils';
import NProgress from 'nprogress';
import { defineComponent, ref, onUnmounted } from 'vue';
import AdminNavigation from '@/components/Admin/Navigation.vue';

async function load(to: any, from: any, next: any) {
    NProgress.start();
    NProgress.inc();

    var res = await myFetch(config.apiBase + "/admin/info");

    next((e: any) => e.loading({
        data: res
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
var interval = 0;

function loading(data2: any) {
    data.value = data2.data;

    interval = setInterval(async () => {
        var res = await myFetch(config.apiBase + "/admin/info");
        data.value = res;
    }, 5000);

    loaded.value = true;
}

defineExpose({ loading });

onUnmounted(() => {
    clearInterval(interval);
})

function formatSize(size: number) {
    if (size < 2 * 1024) return size + " B";
    if (size < 2 * 1024 * 1024) return (size / 1024).toFixed(2) + " KB";
    if (size < 2 * 1024 * 1024 * 1024) return (size / 1024 / 1024).toFixed(2) + " MB";
    return (size / 1024 / 1024 / 1024).toFixed(2) + " GB";
}
</script>

<template>
    <div v-if="loaded">
        <AdminNavigation current="index"></AdminNavigation>
        <div class="d-flex" style="gap: 15px;">
            <v-card class="AdminVCard card-radius statistics">
                <v-card-text>
                    <p class="title">Problems</p>
                    <div class="d-flex align-center">
                        <v-icon icon="mdi-clipboard-list" class="icon"></v-icon>
                        <p class="value">{{ data.problems }}</p>
                    </div>
                </v-card-text>
            </v-card>
            <v-card class="AdminVCard card-radius statistics">
                <v-card-text>
                    <p class="title">Total Submissions</p>
                    <div class="d-flex align-center">
                        <v-icon icon="mdi-memory" class="icon"></v-icon>
                        <p class="value">{{ data.totalSubmissions }}</p>
                    </div>
                </v-card-text>
            </v-card>
            <v-card class="AdminVCard card-radius statistics">
                <v-card-text>
                    <p class="title">Today's Submissions</p>
                    <div class="d-flex align-center">
                        <v-icon icon="mdi-memory" class="icon"></v-icon>
                        <p class="value">{{ data.todaySubmissions }}</p>
                    </div>
                </v-card-text>
            </v-card>
        </div>
        <div class="d-flex" style="gap: 15px;">
            <v-card class="AdminVCard card-radius statistics">
                <v-card-text>
                    <p class="title">Contests</p>
                    <div class="d-flex align-center">
                        <v-icon icon="mdi-calendar-blank" class="icon"></v-icon>
                        <p class="value">{{ data.contests }}</p>
                    </div>
                </v-card-text>
            </v-card>
            <v-card class="AdminVCard card-radius statistics">
                <v-card-text>
                    <p class="title">Users</p>
                    <div class="d-flex align-center">
                        <v-icon icon="mdi-account" class="icon"></v-icon>
                        <p class="value">{{ data.users }}</p>
                    </div>
                </v-card-text>
            </v-card>
            <v-card class="AdminVCard card-radius statistics">
                <v-card-text>
                    <p class="title">User Groups</p>
                    <div class="d-flex align-center">
                        <v-icon icon="mdi-account-group" class="icon"></v-icon>
                        <p class="value">{{ data.groups }}</p>
                    </div>
                </v-card-text>
            </v-card>
        </div>
        <div class="d-flex" style="gap: 15px;">
            <v-card class="AdminVCard card-radius statistics">
                <v-card-text>
                    <p class="title">Tags</p>
                    <div class="d-flex align-center">
                        <v-icon icon="mdi-tag" class="icon"></v-icon>
                        <p class="value">{{ data.tags }}</p>
                    </div>
                </v-card-text>
            </v-card>
        </div>
        <v-card class="AdminVCard card-radius">
            <v-card-title>CPU Info</v-card-title>
            <v-card-text>
                <div class="d-flex align-center">
                    <p style="flex-shrink: 0;">Model Name：</p>
                    <v-text-field 
                        v-model="data.cpu.name"
                        disabled
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mt-3">
                    <p style="flex-shrink: 0;">Cores：</p>
                    <v-text-field 
                        :model-value="data.cpu.cores + ' Cores'"
                        disabled
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mt-3">
                    <p style="flex-shrink: 0;">Speed：</p>
                    <v-text-field 
                        :model-value="data.cpu.speed / 1000 + ' GHz'"
                        disabled
                        dense
                        hide-details
                        clearable
                        density="compact"
                    ></v-text-field>
                </div>
                <div class="d-flex align-center mt-3" style="height: 40px">
                    <p style="flex-shrink: 0;">Usage：</p>
                    <v-progress-linear
                        bg-color="gray"
                        buffer-color="blue"
                        buffer-opacity="1"
                        :buffer-value="data.cpu.usage.sy + data.cpu.usage.us"
                        color="orange"
                        height="12"
                        max="100"
                        min="0"
                        :model-value="data.cpu.usage.us"
                        rounded
                    ></v-progress-linear>
                    <p style="flex-shrink: 0; margin-left: 10px;">
                        User：{{ Math.round(data.cpu.usage.us * 10) / 10 }}% |
                        System：{{ Math.round(data.cpu.usage.sy * 10) / 10 }}% |
                        Free：{{ Math.round((100 - data.cpu.usage.us - data.cpu.usage.sy) * 10) / 10 }}%
                    </p>
                </div>
            </v-card-text>
        </v-card>
        <v-card class="AdminVCard card-radius">
            <v-card-title>Memory Info</v-card-title>
            <v-card-text>
                <div class="d-flex align-center" style="height: 40px">
                    <p style="flex-shrink: 0;">Memory：</p>
                    <v-progress-linear
                        bg-color="gray"
                        color="orange"
                        height="12"
                        :max="data.memory.mem.total"
                        min="0"
                        :model-value="data.memory.mem.used"
                        rounded
                    ></v-progress-linear>
                    <p style="flex-shrink: 0; margin-left: 10px;">
                        {{ formatSize(data.memory.mem.used * 1024 * 1024) }} /
                        {{ formatSize(data.memory.mem.total * 1024 * 1024) }}
                    </p>
                </div>
                <div class="d-flex align-center" style="height: 40px">
                    <p style="flex-shrink: 0;">Swap Memory：</p>
                    <v-progress-linear
                        bg-color="gray"
                        color="orange"
                        height="12"
                        :max="data.memory.swap.total"
                        min="0"
                        :model-value="data.memory.swap.used"
                        rounded
                    ></v-progress-linear>
                    <p style="flex-shrink: 0; margin-left: 10px;">
                        {{ formatSize(data.memory.swap.used * 1024 * 1024) }} /
                        {{ formatSize(data.memory.swap.total * 1024 * 1024) }}
                    </p>
                </div>
            </v-card-text>
        </v-card>
        <v-card class="AdminVCard card-radius">
            <v-card-title>Disk Info</v-card-title>
            <v-card-text>
                <div class="d-flex align-center" style="height: 40px" v-for="item in data.disk">
                    <p style="flex-shrink: 0; font-family: 'Cascadia Mono', monospace!important;">{{ item.mounted }}：</p>
                    <v-progress-linear
                        bg-color="gray"
                        color="orange"
                        height="12"
                        :max="item.used + item.avail"
                        min="0"
                        :model-value="item.used"
                        rounded
                    ></v-progress-linear>
                    <p style="flex-shrink: 0; margin-left: 10px;">
                        {{ formatSize(item.used) }} /
                        {{ formatSize(item.used + item.avail) }}
                    </p>
                </div>
            </v-card-text>
        </v-card>
    </div>
</template>

<style scoped>
.AdminVCard {
    color: var(--color-text)!important;
    background-color: var(--color-background)!important;
    margin-bottom: 20px;
    padding: 5px 10px 0px 10px;
}

.statistics {
    width: calc((100% - 30px) / 3);
}

.statistics .title {
    font-size: 1.5rem;
}

.statistics .icon {
    width: 0.7rem;
}

.statistics .value {
    font-size: 4rem;
    margin-left: 10px;
    font-family: 'Cascadia Mono', monospace!important;
}
</style>