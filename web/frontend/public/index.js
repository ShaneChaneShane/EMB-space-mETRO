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

    switch(light) {
        case "very_high":
            break;
        case "high":
            break;
        case "moderate":
            break;
    }

}

function refresh(text = "") {

}

function pushActive(status) {

}

function pushNotification() {

}