// Global variables
let spoolmanUrl = '';
let spoolsData = [];

// Auxiliary functions for data manipulation
function processSpoolData(data) {
    return data.map(spool => ({
        id: spool.id,
        remaining_weight: spool.remaining_weight,
        remaining_length: spool.remaining_length,
        filament: spool.filament,
        extra: spool.extra
    }));
}

// Dropdown functions
function populateVendorDropdown(data, selectedSmId = null) {
    const vendorSelect = document.getElementById("vendorSelect");
    if (!vendorSelect) {
        console.error('vendorSelect Element nicht gefunden');
        return;
    }
    const onlyWithoutSmId = document.getElementById("onlyWithoutSmId");
    if (!onlyWithoutSmId) {
        console.error('onlyWithoutSmId Element nicht gefunden');
        return;
    }

    // Separate objects for all manufacturers and filtered manufacturers
    const allVendors = {};
    const filteredVendors = {};

    vendorSelect.innerHTML = '<option value="">Please choose ...</option>';

    let vendorIdToSelect = null;
    let totalSpools = 0;
    let spoolsWithoutTag = 0;
    let totalWeight = 0;
    let totalLength = 0;
    // New object for material grouping
    const materials = {};

    data.forEach(spool => {
        if (!spool.filament || !spool.filament.vendor) {
            return;
        }

        totalSpools++;
        
        // Count and group material
        if (spool.filament.material) {
            const material = spool.filament.material.toUpperCase(); // normalization

            materials[material] = (materials[material] || 0) + 1;
        }

        // Add weight and length
        if (spool.remaining_weight) {
            totalWeight += spool.remaining_weight;
        }
        if (spool.remaining_length) {
            totalLength += spool.remaining_length;
        }

        const vendor = spool.filament.vendor;
        
        const hasValidNfcId = spool.extra && 
                             spool.extra.nfc_id && 
                             spool.extra.nfc_id !== '""' && 
                             spool.extra.nfc_id !== '"\\"\\"\\""';
        
        if (!hasValidNfcId) {
            spoolsWithoutTag++;
        }

        // Collect all manufacturers
        if (!allVendors[vendor.id]) {
            allVendors[vendor.id] = vendor.name;
        }

        // Filtered manufacturer for dropdown
        if (!filteredVendors[vendor.id]) {
            if (!onlyWithoutSmId.checked || !hasValidNfcId) {
                filteredVendors[vendor.id] = vendor.name;
            }
        }
    });

    // After the loop: formatting of the total length
    console.log("Total Lenght: ", totalLength);
    const formattedLength = totalLength > 1000 
        ? (totalLength / 1000).toFixed(2) + " km" 
        : totalLength.toFixed(2) + " m";

    // Formatting of the total weight (from g to kg to t)
    const weightInKg = totalWeight / 1000;  // Only convert into KG
    const formattedWeight = weightInKg > 1000 
        ? (weightInKg / 1000).toFixed(2) + " t" 
        : weightInKg.toFixed(2) + " kg";

    // Fill the dropdown with filtered manufacturers
    Object.entries(filteredVendors).forEach(([id, name]) => {
        const option = document.createElement("option");
        option.value = id;
        option.textContent = name;
        vendorSelect.appendChild(option);
    });

    document.getElementById("totalSpools").textContent = totalSpools;
    document.getElementById("spoolsWithoutTag").textContent = spoolsWithoutTag;
    // Zeige die Gesamtzahl aller Hersteller an
    document.getElementById("totalVendors").textContent = Object.keys(allVendors).length;
    
    // Add new statistics
    document.getElementById("totalWeight").textContent = formattedWeight;
    document.getElementById("totalLength").textContent = formattedLength;

    // Add material statistics to DOM
    const materialsList = document.getElementById("materialsList");
    materialsList.innerHTML = '';
    Object.entries(materials)
        .sort(([,a], [,b]) => b - a) // Sort by count in descending order.
        .forEach(([material, count]) => {
            const li = document.createElement("li");
            li.textContent = `${material}: ${count} ${count === 1 ? 'Spool' : 'Spools'}`;
            materialsList.appendChild(li);
        });

    if (vendorIdToSelect) {
        vendorSelect.value = vendorIdToSelect;
        updateFilamentDropdown(selectedSmId);
    }
}

function updateFilamentDropdown(selectedSmId = null) {
    const vendorId = document.getElementById("vendorSelect").value;
    const dropdownContentInner = document.getElementById("filament-dropdown-content");
    const filamentSection = document.getElementById("filamentSection");
    const onlyWithoutSmId = document.getElementById("onlyWithoutSmId").checked;
    const selectedText = document.getElementById("selected-filament");
    const selectedColor = document.getElementById("selected-color");

    dropdownContentInner.innerHTML = '';
    selectedText.textContent = "Please choose ...";
    selectedColor.style.backgroundColor = '#FFFFFF';

    if (vendorId) {
        const filteredFilaments = spoolsData.filter(spool => {
            const hasValidNfcId = spool.extra && 
                                 spool.extra.nfc_id && 
                                 spool.extra.nfc_id !== '""' && 
                                 spool.extra.nfc_id !== '"\\"\\"\\""';
            
            return spool.filament.vendor.id == vendorId && 
                   (!onlyWithoutSmId || !hasValidNfcId);
        });

        filteredFilaments.forEach(spool => {
            const option = document.createElement("div");
            option.className = "dropdown-option";
            option.setAttribute("data-value", spool.filament.id);
            option.setAttribute("data-nfc-id", spool.extra.nfc_id || "");
            
            const colorHex = spool.filament.color_hex || 'FFFFFF';
            option.innerHTML = `
                <div class="option-color" style="background-color: #${colorHex}"></div>
                <span>${spool.id} | ${spool.filament.name} (${spool.filament.material})</span>
            `;
            
            option.onclick = () => selectFilament(spool);
            dropdownContentInner.appendChild(option);
        });

        filamentSection.classList.remove("hidden");
    } else {
        filamentSection.classList.add("hidden");
    }
}

function selectFilament(spool) {
    const selectedColor = document.getElementById("selected-color");
    const selectedText = document.getElementById("selected-filament");
    const dropdownContent = document.getElementById("filament-dropdown-content");
    
    selectedColor.style.backgroundColor = `#${spool.filament.color_hex || 'FFFFFF'}`;
    selectedText.textContent = `${spool.id} | ${spool.filament.name} (${spool.filament.material})`;
    dropdownContent.classList.remove("show");
    
    document.dispatchEvent(new CustomEvent('filamentSelected', { 
        detail: spool 
    }));
}

// Initialization and event handler
async function initSpoolman() {
    try {
        const response = await fetch('/api/url');
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const data = await response.json();
        if (!data.spoolman_url) {
            throw new Error('spoolman_url not found in the response');
        }
        
        spoolmanUrl = data.spoolman_url;
        
        const fetchedData = await fetchSpoolData();
        spoolsData = processSpoolData(fetchedData);
        
        document.dispatchEvent(new CustomEvent('spoolDataLoaded', { 
            detail: spoolsData 
        }));
    } catch (error) {
        console.error('Error in initializing Spoolman:', error);
        document.dispatchEvent(new CustomEvent('spoolmanError', { 
            detail: { message: error.message } 
        }));
    }
}

async function fetchSpoolData() {
    try {
        if (!spoolmanUrl) {
            throw new Error('Spoolman URL is not initialized');
        }
        
        const response = await fetch(`${spoolmanUrl}/api/v1/spool`);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        const data = await response.json();
        return data;
    } catch (error) {
        console.error('Error when calling up the spool data:', error);
        return [];
    }
}

/*
// Export functions
window.getSpoolData = () => spoolsData;
window.reloadSpoolData = initSpoolman;
window.populateVendorDropdown = populateVendorDropdown;
window.updateFilamentDropdown = updateFilamentDropdown;
window.toggleFilamentDropdown = () => {
    const content = document.getElementById("filament-dropdown-content");
    content.classList.toggle("show");
};
*/

// Event Listener
document.addEventListener('DOMContentLoaded', () => {
    initSpoolman();
    
    const vendorSelect = document.getElementById('vendorSelect');
    if (vendorSelect) {
        vendorSelect.addEventListener('change', () => updateFilamentDropdown());
    }
    
    const onlyWithoutSmId = document.getElementById('onlyWithoutSmId');
    if (onlyWithoutSmId) {
        onlyWithoutSmId.addEventListener('change', () => {
            populateVendorDropdown(spoolsData);
            updateFilamentDropdown();
        });
    }
    
    document.addEventListener('spoolDataLoaded', (event) => {
        populateVendorDropdown(event.detail);
    });
    
    window.onclick = function(event) {
        if (!event.target.closest('.custom-dropdown')) {
            const dropdowns = document.getElementsByClassName("dropdown-content");
            for (let dropdown of dropdowns) {
                dropdown.classList.remove("show");
            }
        }
    };

    const refreshButton = document.getElementById('refreshSpoolman');
    if (refreshButton) {
        refreshButton.addEventListener('click', async () => {
            try {
                refreshButton.disabled = true;
                refreshButton.textContent = 'Wird aktualisiert...';
                await initSpoolman();
                refreshButton.textContent = 'Refresh Spoolman';
            } finally {
                refreshButton.disabled = false;
            }
        });
    }
});

// Export functions
window.getSpoolData = () => spoolsData;
window.setSpoolData = (data) => { spoolsData = data; };
window.reloadSpoolData = initSpoolman;
window.populateVendorDropdown = populateVendorDropdown;
window.updateFilamentDropdown = updateFilamentDropdown;
window.toggleFilamentDropdown = () => {
    const content = document.getElementById("filament-dropdown-content");
    content.classList.toggle("show");
};

// Event Listener for Click outside dropdown
window.onclick = function(event) {
    if (!event.target.closest('.custom-dropdown')) {
        const dropdowns = document.getElementsByClassName("dropdown-content");
        for (let dropdown of dropdowns) {
            dropdown.classList.remove("show");
        }
    }
};