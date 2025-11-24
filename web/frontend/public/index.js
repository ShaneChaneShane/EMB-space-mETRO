/* 
    2 main function
    1. Refresh 
        - Use when
            - Refresh button is clicked
            - After engine state is changed
        - What did it do
            - Fetch new data from Backend
            - Remark 'Last update' with current date
            - Pop up 'Refresh successfully' notification
    2. Change state
        - Use when
            - Active/Inactive button are clicked
        - What did it do
            - Send API to Backend about changing state
            - Switch state card to processing mode until receive response from Backend
            - After receiving response -> show pop-up and soft refresh 
*/

function updateStatusCard(status) {

    // pull items from document 
    const statusCard = document.getElementById('status-card');
    const statusText = document.getElementById('status-text');
    const statusButton = document.getElementById('status-botton');
    const statusButtonText = document.getElementById('status-botton-text');

    // -> Active
    if (status == 'active') {
        statusCard.classList.remove("color-inactive-state")
        statusCard.classList.remove("color-process-state")
        statusCard.classList.add("color-active-state")

        statusButton.classList.remove("opacity-0")
        statusButton.classList.add("opacity-100")
        
        statusText.textContent = 'Active';
        statusButtonText.textContent = 'Deactivate' 
    }

    //-> Forward
    else if (status == 'forward') {
        statusCard.classList.remove("color-inactive-state")
        statusCard.classList.remove("color-active-state")
        statusCard.classList.add("color-process-state")

        statusText.textContent = 'Processing...';
        
        statusButton.classList.remove("opacity-100")
        statusButton.classList.add("opacity-0")
    }

    // -> InActive
    else if (status == 'inactive') {
        statusCard.classList.remove("color-active-state")
        statusCard.classList.remove("color-process-state")
        statusCard.classList.add("color-inactive-state")

        statusButton.classList.remove("opacity-0")
        statusButton.classList.add("opacity-100")
        
        statusText.textContent = 'Inactive';
        statusButtonText.textContent = 'Activate'
    }
}

function updateRaingCard(isRaining = false, temp = 0, humid = 0) {

    // pull items from document 
    const rainingCard = document.getElementById("raining-card");
    const rainingTemp = document.getElementById("raining-temp");
    const rainingState = document.getElementById("raining-state");
    const rainingHumid = document.getElementById("raining-humid");
    const rainingCloud = document.getElementById("raining-cloud");

    // -> Raining
    if (isRaining) {
        rainingCard.classList.remove("color-nraining");
        rainingCard.classList.add("color-raining");

        rainingState.textContent = "Raining";

        rainingCloud.src = "img/cloud-raining.svg";
    }

    // -> Not raining
    else {
        rainingCard.classList.remove("color-raining");
        rainingCard.classList.add("color-nraining");

        rainingState.textContent = "Not raining";

        rainingCloud.src = "img/cloud-nraining.svg";
    }

    rainingTemp.textContent = temp + " °C";
    rainingHumid.textContent = "Humidity: " + humid + "%";

}

function updateDrynessCard(dryness) {

    // pull items from document 
    // const drynessCard = document.getElementById("dryness-card");
    const drynessScore = document.getElementById("dryness-score");
    const drynessImg = document.getElementById("dryness-img");

    if (typeof dryness === 'number'  && dryness >= 1 && dryness <= 5) {
        drynessScore.textContent = dryness;
        drynessImg.src = "img/shirt-"+dryness+".svg";
    } else {
        drynessScore.textContent = "-";
    }

}

function updateLightCard(light) {

    const lightCard = document.getElementById("light-card");
    const lightStatus = document.getElementById("light-status");
    const exp = document.getElementById("explosion-frame");

    switch(light) {
        case "very_high":

            lightStatus.textContent = "Highhhhhh";

            lightCard.classList.remove("light-card--high", "light-card--mid", "light-card--low", "light-card--none")
            lightCard.classList.add("light-card--high")

            lightCard.classList.remove("opacity-100", "duration-500")
            lightCard.classList.add("animate-spin", "opacity-0", "duration-1000");
            addTemporaryExplosion(exp);

            break;

        case "high":

            lightCard.classList.remove("animate-spin", "opacity-0", "duration-1000");

            lightCard.classList.remove("light-card--high", "light-card--mid", "light-card--low", "light-card--none")
            lightCard.classList.add("opacity-100", "light-card--high")

            lightStatus.textContent = "High";

            break;

        case "moderate":

            lightCard.classList.remove("animate-spin", "opacity-0", "duration-1000");

            lightCard.classList.remove("light-card--high", "light-card--mid", "light-card--low", "light-card--none")
            lightCard.classList.add("opacity-100", "light-card--mid")

            lightStatus.textContent = "Mid";

            break;

        case "low":
            
            lightCard.classList.remove("animate-spin", "opacity-0", "duration-1000");

            lightCard.classList.remove("light-card--high", "light-card--mid", "light-card--low", "light-card--none")
            lightCard.classList.add("opacity-100", "duration-500", "light-card--low")

            lightStatus.textContent = "Low";

            break;

        case "no_sun":

            lightCard.classList.remove("animate-spin", "opacity-0", "duration-1000");

            lightCard.classList.remove("light-card--high", "light-card--mid", "light-card--low", "light-card--none")
            lightCard.classList.add("opacity-100", "duration-500", "light-card--none")

            lightStatus.textContent = "None";

            break;

        default:
            lightCard.classList.remove("animate-spin", "opacity-0", "duration-1000");

            lightCard.classList.remove("light-card--high", "light-card--mid", "light-card--low", "light-card--none")
            lightCard.classList.add("opacity-100", "light-card--none")

            lightStatus.textContent = "-";
    }

}

function addTemporaryExplosion(targetElement) {
    const explosion = document.createElement('div');
    explosion.className = 'absolute inset-0 z-50 flex items-center justify-center pointer-events-none';
    explosion.innerHTML = `
        <img src="img/boom-explosion.gif" class="w-full h-full" />
    `;
    
    targetElement.appendChild(explosion);
    
    setTimeout(() => {
        explosion.remove();
    }, 800);
}

// === API call === //

const API_BASE_URL = 'http://localhost:5000/api';

async function getWeatherStatus() {

    try {
        const response = await fetch(`${API_BASE_URL}/weather`);
        const data = await response.json();
        
        if (response.status == 200) {
            return data;
        } else {
            throw new Error(data.message || 'Failed to get weather status');
        }
        
    } catch (error) {
        console.error('Get weather status failed:', error);
        throw new Error(data.message || 'Failed to get weather status');
    }

}

async function getProtectorStatus() {

    try {
        const response = await fetch(`${API_BASE_URL}/rainProtector/status`);
        const data = await response.json();
        
        if (response.status == 200) {
            return data;
        } else {
            throw new Error(data.message || 'Failed to get protector status');
        }
        
    } catch (error) {
        console.error('Get protector status failed:', error);
        throw new Error(data.message || 'Failed to get protector status');
    }

}

async function updateRainProtector(status) {

    try {
        const response = await fetch(`${API_BASE_URL}/rainProtector`, {
            method: 'PUT',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify({
                shouldOpen: status  
            })
        });
        
        const data = await response.json();
        
        if (response.status == 200 && data.success) {
            return data;
        } else {
            throw new Error(data.message || 'Failed to update protector');
        }
        
    } catch (error) {
        console.error('Update protector failed:', error);
        throw error;
    }

}

// === Helped function === //

function loadingScreen(state = "") {
    const base = document.getElementById("base");

    if (state == "start") { 
        base.classList.add("animate-spin", "cursor-not-allowed", "pointer-events-none", "select-none");

        // for test
        // setTimeout(() => {
        //     base.classList.remove("animate-spin", "cursor-not-allowed", "pointer-events-none", "select-none");
        // }, 2500);

    } else if (state == "stop") {
        base.classList.remove("animate-spin", "cursor-not-allowed", "pointer-events-none", "select-none");
    }
    
}

function pushNotification(txt, isGood = true) {

    const color = (isGood) ? "text-green-800" : "text-red-800"

    // const noti = document.createElement('p');
    // noti.className = 'absolute bottom-20 duration-1000 text-[8px]' + color;
    // noti.textContent = txt;
    
    // const bttm = document.getElementById("bottom-box");
    // bttm.appendChild(noti);
    
    // setTimeout(() => {
    //     noti.classList.add("translate-x-100")
    // }, 50);

    const bttm = document.getElementById("last-update");
    const update = bttm.textContent;

    bttm.classList.remove("text-(--tcolor-base1)");
    bttm.classList.add("font-bold", color);
    bttm.textContent = txt;

    setTimeout(() => {
        bttm.classList.remove("font-bold", color);
        bttm.classList.add("text-(--tcolor-base1)");
        bttm.textContent = update;
    }, 5000);

}

function getCurrentDisplayState() {
    const statusText = document.getElementById('status-text').textContent.toLowerCase();
    return statusText; // active/inactive
}

// === Main function === //

async function refresh() {

    console.log("refresh")

    try {
        // loading screen
        loadingScreen("start");

        // get data
        const weatherData = await getWeatherStatus();
        const protectorData = await getProtectorStatus();
        
        const humid = weatherData.data.humidity || 0;
        const isRaining = weatherData.data.rain == 'raining' || false;

        const light = weatherData.data.sun || 0;
        const temp = weatherData.data.temperature || 0;
        const dryness = weatherData.data.dryness || 0;

        const status = protectorData.data.isOpen ? 'active' : 'inactive'

        setTimeout(() =>{
            console.log("timeout to show my fabulous loading screen");

            // exit loading screen
            loadingScreen("stop");

            // update UI
            updateStatusCard(status);
            updateRaingCard(isRaining, temp, humid);
            updateDrynessCard(dryness);
            updateLightCard(light);

            // last update
            document.getElementById('last-update').textContent = 'Last update: ' + new Date().toLocaleString();

            // push notification
            pushNotification("Refresh successully", true);
        } , 1500)

    } catch (error) {
        console.error("Refresh Error: ", error);    
        
        setTimeout(() =>{
            console.log("timeout to show my fabulous loading screen");

            // exit loading screen
            loadingScreen("stop"); 
            updateStatusCard("forward");

            pushNotification("Cannot fetch data", false);
        } , 1500)
    }

}

async function changeProtectorState() {

    try {
        loadingScreen("start");

        // Get current state
        const current = getCurrentDisplayState();
        const toState = (current == "active") ? false : true;

        const result = await updateRainProtector(toState);
        
        console.log('Protector update result:', result);

        setTimeout(async () => {
            await refresh();
        }, 1000)

        pushNotification(`Status changed successfully`, true);

    } catch (error) {
        await refresh();

        console.error('Change protector state failed:', error);
        pushNotification("Cannot change protector state", false);
    }

}

// for testing
async function test1() {

    // loading screen
    loadingScreen("start");

    setTimeout(() => {
        loadingScreen("stop")

        // update UI
        updateStatusCard("inactive");
        updateRaingCard(true, 100, 0.8);
        updateDrynessCard(3);
        updateLightCard("very_high");

        // last update
        document.getElementById('last-update').textContent = 'Last update: ' + new Date().toLocaleString();

        // push notification
        pushNotification("Refresh successully", true);
        }, 1500);

}

async function test2() {

    // loading screen
    loadingScreen("start");

    setTimeout(() => {
        loadingScreen("stop")

        // update UI
        updateStatusCard("active");
        updateRaingCard(false, 8, 10);
        updateDrynessCard(1);
        updateLightCard("moderate");

        // last update
        document.getElementById('last-update').textContent = 'Last update: ' + new Date().toLocaleString();

        // push notification
        pushNotification("Refresh successully", true);
        }, 1500);

}

// Auto refresh at start
document.addEventListener('DOMContentLoaded', function() {
    console.log('Page loaded - Starting automatic refresh...');   
    refresh();
});




