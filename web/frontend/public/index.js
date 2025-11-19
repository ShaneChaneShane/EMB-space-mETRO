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
    rainingHumid.textContent = "Humidity: " + humid*100 + "%";

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

const API_BASE_URL = 'http://localhost:3000/api';

async function getWeatherStatus() {

    try {
        const response = await fetch(`${API_BASE_URL}/`);
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
        const response = await fetch(`${API_BASE_URL}/rainProtector/status/`);
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

    // structure แป่ก ๆ
    try {
        const response = await fetch(`${API_BASE_URL}/rainProtector/`);
        const data = await response.json();
        
        if (response.status == 200) {
            return data;
        } else {
            throw new Error(data.message || 'Failed to update protector');
        }
        
    } catch (error) {
        console.error('Update protector failed:', error);
        throw new Error(data.message || 'Failed to update protector');
    }


}

// === Helped function === //

function loadingScreen(state = "") {

}

function pushNotification(isGood, text) {

}

function getCurrentDisplayState() {
    const statusText = document.getElementById('status-text').textContent.toLowerCase();
    return statusText; 

    // active/inactive
}

// === Main function === //

async function refresh() {

    // loading screen
    loadingScreen("start");

    // get data
    try {
        const weatherData = await getWeatherStatus();
        const protectorData = await getProtectorStatus();
    } catch (error) {
        pushNotification(false, "Cannot fetch data")
    }
    
    const humid = weatherData.data.humidity || 0;
    const isRaining = weatherData.data.raining;
    isRaining = (isRaining == "raining") ? true : false;

    const light = weatherData.data.sun || "";
    const temp = weatherData.data.temperature || 0;
    const dryness = weatherData.data.dryness || 0;

    const status = protectorData.data.isOpen; // active, inactive, process

    // update UI
    updateStatusCard(status);
    updateRaingCard(isRaining, temp, humid);
    updateDrynessCard(dryness);
    updateLightCard(light);

    // push notification
    pushNotification(true, "Refresh successully");

    // exit loading screen
    loadingScreen("stop");

}

async function changeProtectorState(current) {

    toState = (current == "active") ? "inactive" : "active" ;

    try {
        const res = updateRainProtector(toState);
    } catch (error) {
        pushNotification(false, "Cannot change protector state")
    }
    
    await refresh();

}


